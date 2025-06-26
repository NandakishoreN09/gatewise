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
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "CommonDefines.h"
#include "GPIO.h"
#include "OVERLAY.h"

// Constructor
GPIO::GPIO() {
    for (int count = 0; count < 128; count++)
        gpioPin[count] = unexported;
}

// Export GPIO pin
int GPIO::exportPin(uint8_t pin) {
    if (gpioPin[pin] == exported)
        return -1;

    FILE *fd;
    if ((fd = fopen("/sys/class/gpio/export", "w")) == NULL) {
        perror("GPIO export failed");
        return -1;
    }
    if (fprintf(fd, "%d", pin) < 0) {
        perror("GPIO export failed");
        fclose(fd);
        return -1;
    }
    fclose(fd);
    usleep(100000); // Add a delay of 100ms to allow kernet to create GPIO files
    gpioPin[pin] = exported; // Mark as exported
    return pin;
}

// Unexport GPIO pin
int GPIO::unexportPin(uint8_t pin) {
    if (gpioPin[pin] == unexported)
        return -1;

    FILE *fd;
    if ((fd = fopen("/sys/class/gpio/unexport", "w")) == NULL) {
        perror("GPIO unexport failed");
        return -1;
    }
    if (fprintf(fd, "%d", pin) < 0) {
        perror("GPIO unexport failed");
        fclose(fd);
        return -1;
    }
    fclose(fd);
    gpioPin[pin] = unexported; // Mark as unexported
    return pin;
}

// Set GPIO pin direction
int GPIO::setDirection(uint8_t pin, uint8_t direction) {
    FILE *fd;
    char path[34];
    sprintf(path, "/sys/class/gpio/gpio%d/direction", pin);

    if ((fd = fopen(path, "w")) == NULL) {
        std::cerr << "GPIO set direction failed for pin " << pin << ": " << strerror(errno) << std::endl;
        return -1;
    }

    switch (direction) {
        case INPUT:
            fprintf(fd, "in");
            break;
        case OUTPUT:
            fprintf(fd, "out");
            break;
        default:
            std::cerr << "Invalid direction for pin " << pin << std::endl;
            fclose(fd);
            return -1;
    }

    fclose(fd);
    return 0;
}

// Configure GPIO pin
int GPIO::gpioConfig(uint8_t pin, uint8_t direction) {
    std::cout << "Configuring GPIO pin " << (int)pin << " as "
              << (direction == INPUT ? "INPUT" : "OUTPUT") << std::endl;

    // Check if the pin number is valid
    if (pin >= MAX_GPIO_PINS) {
        std::cerr << "Invalid GPIO pin number: " << (int)pin << std::endl;
        return -1;
    }

    // Ensure the GPIO pin is unexported first (cleanup previous state)
    if (this->unexportPin(pin) == 0) {
        std::cout << "GPIO pin " << (int)pin << " unexported successfully (cleanup step)." << std::endl;
    }

    // Export the GPIO pin
    if (this->exportPin(pin) < 0) {
        std::cerr << "Failed to export pin " << (int)pin << std::endl;
        return -1;
    }
    gpioPin[pin] = exported;

    // Allow time for the kernel to create GPIO files
    usleep(100000); // Add a delay (100ms)

    // Set the GPIO direction
    FILE* fd;
    char path[34];
    sprintf(path, "/sys/class/gpio/gpio%d/direction", pin);

    if ((fd = fopen(path, "w")) == NULL) {
        std::cerr << "Failed to open direction file for pin " << (int)pin
                  << ": " << strerror(errno) << std::endl;
        return -1;
    }

    switch (direction) {
        case INPUT:
            fprintf(fd, "in");
            break;
        case OUTPUT:
            fprintf(fd, "out");
            break;
        default:
            std::cerr << "Invalid direction for pin " << (int)pin << std::endl;
            fclose(fd);
            return -1;
    }

    fclose(fd);

    std::cout << "GPIO pin " << (int)pin << " configured as "
              << (direction == INPUT ? "INPUT" : "OUTPUT") << std::endl;

    return 0; // Success
}

// Write to GPIO pin
int GPIO::writeValue(uint8_t pin, uint8_t value) {
    FILE *fd;
    char path[30];
    sprintf(path, "/sys/class/gpio/gpio%d/value", pin);

    if ((fd = fopen(path, "w")) == NULL) {
        perror("GPIO write failed");
        return -1;
    }

    fprintf(fd, "%d", value);
    fclose(fd);
    return value;
}

// Read from GPIO pin
uint8_t GPIO::readValue(uint8_t pin) {
    FILE *fd;
    int value;
    char path[30];
    sprintf(path, "/sys/class/gpio/gpio%d/value", pin);

    if ((fd = fopen(path, "r")) == NULL) {
        std::cerr << "GPIO read failed for pin " << (int)pin << ": " << strerror(errno) << std::endl;
        return -1;
    }

    fscanf(fd, "%d", &value);
    fclose(fd);
    return (uint8_t)value;
}

// Destructor
GPIO::~GPIO() {
    for (int count = 0; count < 128; count++) {
        if (gpioPin[count] == exported) {
            this->writeValue(count, 0);
            this->unexportPin(count);
	    std::cout << "GPIO pin " << count << "unexported successfully." << std::endl;
        }
    }
}

// Singleton instance of GPIO
GPIO* _gpio = nullptr;

GPIO* gpioInstance() {
    if (!_gpio) {
        std::cout << "Creating GPIO instance..." << std::endl;
        _gpio = new GPIO();
        if (!_gpio) {
            std::cerr << "Error: Failed to allocate memory for GPIO instance" << std::endl;
        }
    } else {
        std::cout << "GPIO instance already exists." << std::endl;
    }
    return _gpio;
}

int digitalWrite(Pin pin, bool state) {
    if (!_gpio) {
        std::cerr << "Error: _gpio is null in digitalWrite!" << std::endl;
        return -1;
    }
    int result = _gpio->writeValue(pin.pinNum, state);
    if (result < 0) {
        std::cerr << "GPIO write failed for pin " << pin.pinNum << std::endl;
    }
    return result;
}

uint8_t digitalRead(Pin pin) {
    if (!_gpio) {
        std::cerr << "Error: _gpio is null in digitalRead!" << std::endl;
        return -1;
    }
    return _gpio->readValue(pin.pinNum);
}

