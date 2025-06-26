/*
    This file is a part of the wiringBone library
    Copyright (C) 2015 Abhraneel Bera

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

// Standard header files
// Standard header files
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <wordexp.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <linux/version.h>
#include <string>
#include <mutex> // Added for thread safety
#include "PINS.h"
#include "CommonDefines.h"
#include "OVERLAY.h"
#include "GPIO.h"
#include "PWM.h"
#include "CLOCK.h"

#if LINUX_VERSION_CODE > KERNEL_VERSION(3,8,13)
#define OCPDIR "/sys/devices/platform/ocp/ocp:%s_pinmux/state"
#define SLOTS "/sys/devices/platform/bone_capemgr/slots"
#else
#define OCPDIR "/sys/devices/ocp.*/%s_pinmux.*/state"
#define SLOTS "/sys/devices/bone_capemgr.*/slots"
#endif

std::mutex overlayMutex; // Mutex for thread-safe operations

OVERLAY::OVERLAY() {
    std::lock_guard<std::mutex> lock(overlayMutex); // Ensure thread safety

    _gpio = gpioInstance(); // Ensure this is present
    if (!_gpio) {
        std::cerr << "Error: Failed to initialize GPIO instance" << std::endl;
        exit(EXIT_FAILURE);
    }
    _pru = pruInstance();
    if (!_pru) {
        std::cerr << "Error: Failed to initialize PRU instance" << std::endl;
        exit(EXIT_FAILURE);
    }
    _pwm = pwmInstance();
    if (!_pwm) {
        std::cerr << "Error: Failed to initialize PWM instance" << std::endl;
        exit(EXIT_FAILURE);
    }
    configureDefaultPins();
    startClock();
}

OVERLAY::~OVERLAY() {
    std::lock_guard<std::mutex> lock(overlayMutex); // Ensure thread safety
    restoreAllPins();
    delete _gpio;
    delete _pru;
    delete _pwm;
}

void OVERLAY::configureDefaultPins() {
    std::lock_guard<std::mutex> lock(overlayMutex);
    for (int i = 3; i <= 46; ++i) {
        configOverlay(Pin{static_cast<uint8_t>(i)}); // Configure P8 pins
    }
    for (int i = 11; i <= 42; ++i) {
        configOverlay(Pin{static_cast<uint8_t>(i)}); // Configure P9 pins
    }
}

int OVERLAY::configOverlay(Pin pin) {
    std::lock_guard<std::mutex> lock(overlayMutex);

    if (!isValidMode(pin)) {
        std::cerr << pin.pinName << ": Invalid mode selected" << std::endl;
        return -1;
    }

    if (pin.virtualCape != none) {
        loadVirtualCape(pin);
    }

    std::string filePath = getFilePathForPin(pin);
    FILE* fd = fopen(filePath.c_str(), "w");
    if (!fd) {
        perror((std::string(pin.pinName) + ": Config overlay failed").c_str());
        return -1;
    }

    std::string modeString = getModeString(pin);
    if (fprintf(fd, "%s", modeString.c_str()) < 0) {
        perror((std::string(pin.pinName) + ": Config overlay mode write failed").c_str());
        fclose(fd);
        return -1;
    }
    fclose(fd);
    return 0;
}

void OVERLAY::restoreAllPins() {
    std::lock_guard<std::mutex> lock(overlayMutex);
    for (int i = 3; i <= 46; ++i) {
        restoreOverlay(Pin{static_cast<uint8_t>(i)});
    }
    for (int i = 11; i <= 42; ++i) {
        restoreOverlay(Pin{static_cast<uint8_t>(i)});
    }
}

int OVERLAY::restoreOverlay(Pin pin) {
    std::lock_guard<std::mutex> lock(overlayMutex);
    std::string filePath = getFilePathForPin(pin);
    FILE* fd = fopen(filePath.c_str(), "w");
    if (!fd) {
        perror((std::string(pin.pinName) + ": Restore overlay failed").c_str());
        return -1;
    }
    if (fprintf(fd, "%s", "default") < 0) {
        perror((std::string(pin.pinName) + ": Restore overlay write failed").c_str());
        fclose(fd);
        return -1;
    }
    fclose(fd);
    return 0;
}

bool OVERLAY::isValidMode(Pin pin) {
    for (int i = 0; i < pin.numValidModes; ++i) {
        if (pin.validModes[i] == pin.selectedMode) {
            return true;
        }
    }
    return false;
}

void OVERLAY::loadVirtualCape(Pin pin) {
    if (pin.virtualCape == hdmi) loadCape("cape-univ-hdmi");
    if (pin.virtualCape == audio) loadCape("cape-univ-audio");
    if (pin.virtualCape == emmc) loadCape("cape-univ-emmc");
}

std::string OVERLAY::getFilePathForPin(Pin pin) {
    char filePath[100];
    snprintf(filePath, sizeof(filePath), OCPDIR, pin.pinName.c_str());
    return std::string(filePath);
}

std::string OVERLAY::getModeString(Pin pin) {
    switch (pin.selectedMode) {
        case gpio: return "gpio_pd";
        case pwm: return (pin.pinNum == 113) ? "pwm2" : "pwm";
        case pruin: return "pruin";
        case pruout: return "pruout";
        case i2c: return "i2c";
        case uart: return "uart";
        case spi: return (pin.pinNum == 7) ? "spics" : "spi";
        default: return "";
    }
}

int OVERLAY::loadCape(std::string capeName) {
    std::lock_guard<std::mutex> lock(overlayMutex);
    FILE* fd;
    wordexp_t path;
    wordexp(SLOTS, &path, 0);
    if (capeLoaded(path.we_wordv[0], capeName)) {
        return 0;
    }
    fd = fopen(path.we_wordv[0], "w");
    if (!fd) {
        perror("Cape load failed");
        return -1;
    }
    if (fprintf(fd, "%s", capeName.c_str()) < 0) {
        perror("Cape load failed");
        fclose(fd);
        return -1;
    }
    fclose(fd);
    usleep(1000 * 1000);
    return 0;
}

bool OVERLAY::capeLoaded(const std::string& path, const std::string& capeName) {
    std::lock_guard<std::mutex> lock(overlayMutex);
    std::ifstream slots(path);
    std::string line;
    while (std::getline(slots, line)) {
        if (line.find(capeName) != std::string::npos) {
            return true;
        }
    }
    return false;
}
int pinMode(Pin pin, uint8_t direction) {
    if (!_gpio) {
        std::cerr << "Error: _gpio is null in pinMode!" << std::endl;
        _gpio = gpioInstance(); // Attempt to initialize the GPIO instance
        if (!_gpio) {
            std::cerr << "Critical Error: Failed to initialize _gpio in pinMode!" << std::endl;
            return -1; // Exit if initialization fails
        }
    }

    if (_gpio->gpioConfig(pin.pinNum, direction) < 0) {
        std::cerr << "Failed to configure GPIO pin " << pin.pinNum << std::endl;
        return -1;
    }

    std::cout << "Pin " << pin.pinNum << " configured as "
              << (direction == INPUT ? "INPUT" : "OUTPUT") << std::endl;

    return 0; // Success
}


