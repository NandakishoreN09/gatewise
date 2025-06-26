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

#include "PINS.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>

// Define the mutex
std::mutex gpio_mutex;

// Function to write to a sysfs file with mutex protection
bool write_to_sysfs(const std::string &path, const std::string &value) {
    std::lock_guard<std::mutex> lock(gpio_mutex); // Lock mutex for thread safety
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << path << std::endl;
        return false;
    }
    file << value;
    if (file.fail()) {
        std::cerr << "Failed to write to " << path << std::endl;
        return false;
    }
    return true;
}

// Function to set GPIO direction
bool set_gpio_direction(int pin, const std::string &direction) {
    return write_to_sysfs("/sys/class/gpio/gpio" + std::to_string(pin) + "/direction", direction);
}

// Function to read GPIO value with mutex protection
int read_gpio_value(int pin) {
    std::lock_guard<std::mutex> lock(gpio_mutex);
    std::ifstream file("/sys/class/gpio/gpio" + std::to_string(pin) + "/value");
    if (!file.is_open()) {
        std::cerr << "Failed to open value file for GPIO " << pin << std::endl;
        return -1;
    }
    int value;
    file >> value;
    if (file.fail()) {
        std::cerr << "Failed to read value for GPIO " << pin << std::endl;
        return -1;
    }
    return value;
}

// Function to export GPIO
bool export_gpio(int pin) {
    return write_to_sysfs("/sys/class/gpio/export", std::to_string(pin));
}

// Function to unexport GPIO
bool unexport_gpio(int pin) {
    return write_to_sysfs("/sys/class/gpio/unexport", std::to_string(pin));
}

// Function to initialize a GPIO pin
void initializeGPIO(Pin &pin) {
    // Export the pin
    if (!export_gpio(pin.pinNum)) {
        std::cerr << "Failed to export GPIO pin " << pin.pinName << std::endl;
    }

    // Set the direction of the pin
    if (!set_gpio_direction(pin.pinNum, "in")) {
        std::cerr << "Failed to configure GPIO pin " << pin.pinName << " as INPUT" << std::endl;
    } else {
        std::cout << "GPIO pin " << pin.pinName << " configured as INPUT." << std::endl;
    }
}

// Debug function to verify pin initialization
void debugPinInitialization() {
    std::cout << "Starting Program ..." << std::endl;

}

// Run during library initialization
__attribute__((constructor))
void setupPins() {
    debugPinInitialization();
    if (P8_16.selectedMode != gpio) {
        std::cerr << "Error: P8_16 is not set to GPIO mode!" << std::endl;
    }
}

// Define valid modes for pins
PinModes P8_16_modes[] = {gpio, pruin};
PinModes P8_15_modes[] = {gpio, pruin};
PinModes P8_12_modes[] = {gpio, pruin};
PinModes P8_14_modes[] = {gpio, pruin};
PinModes P8_11_modes[] = {gpio, pruin};

// Define pins with modes
Pin P8_16 = {46, "P8_16", gpio, P8_16_modes, 2, none};
Pin P8_15 = {47, "P8_15", gpio, P8_15_modes, 2, none};
Pin P8_12 = {44, "P8_12", gpio, P8_12_modes, 2, none};
Pin P8_14 = {26, "P8_14", gpio, P8_14_modes, 2, none};
Pin P8_11 = {45, "P8_11", gpio, P8_11_modes, 2, none};


// Define valid modes for pins
//PinModes P8_16_modes[] = {gpio, pruin};
PinModes P9_11_modes[] = {gpio, uart};
PinModes P9_12_modes[] = {gpio};
PinModes P9_13_modes[] = {gpio, uart};
PinModes P9_14_modes[] = {gpio, pwm};
PinModes P9_15_modes[] = {gpio};
PinModes P9_16_modes[] = {gpio, pwm};
PinModes P9_17_modes[] = {gpio, spi, i2c};
PinModes P9_18_modes[] = {gpio, spi, i2c};
PinModes P9_21_modes[] = {gpio, pwm, uart, spi, i2c};
PinModes P9_22_modes[] = {gpio, pwm, uart, spi, i2c};
PinModes P9_23_modes[] = {gpio};
PinModes P9_24_modes[] = {gpio, pruin, uart, i2c};
PinModes P9_25_modes[] = {gpio, pruout, pruin};
PinModes P9_26_modes[] = {gpio, pruin, uart, i2c};
PinModes P9_27_modes[] = {gpio, pruout, pruin};
PinModes P9_28_modes[] = {gpio, pruout, pruin, pwm, spi};
PinModes P9_29_modes[] = {gpio, pruout, pruin, pwm, spi};
PinModes P9_30_modes[] = {gpio, pruout, pruin, spi};
PinModes P9_31_modes[] = {gpio, pruout, pruin, pwm, spi};
PinModes P9_41_modes[] = {gpio, pruout, pruin};
PinModes P9_42_modes[] = {gpio, pruout, pruin, pwm, spi};

// Define default modes for each pin
#define P9_11_MODE gpio
#define P9_12_MODE gpio
#define P9_13_MODE gpio
#define P9_14_MODE gpio
#define P9_15_MODE gpio
#define P9_16_MODE gpio
#define P9_17_MODE gpio
#define P9_18_MODE gpio
#define P9_21_MODE gpio
#define P9_22_MODE gpio
#define P9_23_MODE gpio
#define P9_24_MODE gpio
#define P9_25_MODE gpio
#define P9_26_MODE gpio
#define P9_27_MODE gpio
#define P9_28_MODE gpio
#define P9_29_MODE gpio
#define P9_30_MODE gpio
#define P9_31_MODE gpio
#define P9_41_MODE gpio
#define P9_42_MODE gpio

// Define Pins
Pin P9_11 = {30,  "P9_11", P9_11_MODE, P9_11_modes, 2, none};
Pin P9_12 = {60,  "P9_12", P9_12_MODE, P9_12_modes, 1, none};
Pin P9_13 = {31,  "P9_13", P9_13_MODE, P9_13_modes, 2, none};
Pin P9_14 = {50,  "P9_14", P9_14_MODE, P9_14_modes, 2, none};
Pin P9_15 = {48,  "P9_15", P9_15_MODE, P9_15_modes, 1, none};
Pin P9_16 = {51,  "P9_16", P9_16_MODE, P9_16_modes, 2, none};
Pin P9_17 = {5,   "P9_17", P9_17_MODE, P9_17_modes, 3, none};
Pin P9_18 = {4,   "P9_18", P9_18_MODE, P9_18_modes, 3, none};
Pin P9_21 = {3,   "P9_21", P9_21_MODE, P9_21_modes, 5, none};
Pin P9_22 = {2,   "P9_22", P9_22_MODE, P9_22_modes, 5, none};
Pin P9_23 = {49,  "P9_23", P9_23_MODE, P9_23_modes, 1, none};
Pin P9_24 = {15,  "P9_24", P9_24_MODE, P9_24_modes, 4, none};
Pin P9_25 = {117, "P9_25", P9_25_MODE, P9_25_modes, 3, audio};
Pin P9_26 = {14,  "P9_26", P9_26_MODE, P9_26_modes, 4, none};
Pin P9_27 = {115, "P9_27", P9_27_MODE, P9_27_modes, 3, none};
Pin P9_28 = {113, "P9_28", P9_28_MODE, P9_28_modes, 5, audio};
Pin P9_29 = {111, "P9_29", P9_29_MODE, P9_29_modes, 5, audio};
Pin P9_30 = {112, "P9_30", P9_30_MODE, P9_30_modes, 4, none};
Pin P9_31 = {110, "P9_31", P9_31_MODE, P9_31_modes, 5, audio};
Pin P9_41 = {20,  "P9_41", P9_41_MODE, P9_41_modes, 3, none};
Pin P9_42 = {7,   "P9_42", P9_42_MODE, P9_42_modes, 5, none};
