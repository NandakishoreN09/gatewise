/*
    This file is a part of the wiringBone library
    Updated for improved GPIO handling
*/

#ifndef PINS_H
#define PINS_H

#include <string>
#include <mutex> // For mutex
#include <vector>

enum PinModes { gpio, pruin, pwm, uart, spi, i2c, pruout };
enum VirtualCapes {none, hdmi, audio, emmc };

// Struct for GPIO pin representation
struct Pin {
    int pinNum;                // GPIO number
    std::string pinName;       // Pin name (e.g., "P8_16")
    PinModes selectedMode;     // Currently selected mode
    PinModes *validModes;      // Array of valid modes
    int numValidModes;         // Number of valid modes
    VirtualCapes virtualCape;  // Virtual cape associated
};

// Mutex for GPIO operations
extern std::mutex gpio_mutex;

// Function prototypes
bool write_to_sysfs(const std::string &path, const std::string &value);
bool set_gpio_direction(int pin, const std::string &direction);
int read_gpio_value(int pin);
bool export_gpio(int pin);
bool unexport_gpio(int pin);
void initializeGPIO(Pin &pin);

//Declaration of externally defined PinModes
extern PinModes P8_16_modes[];
extern PinModes P8_15_modes[];
extern PinModes P8_11_modes[];
extern PinModes P8_14_modes[];
extern PinModes P8_12_modes[];
extern PinModes P9_12_modes[];
extern PinModes P9_15_modes[];
extern PinModes P9_23_modes[];
extern PinModes P9_25_modes[];
extern PinModes P9_27_modes[];
extern PinModes P9_41_modes[];

// Declaration of externally defined pins
extern Pin P8_16;
extern Pin P8_15;
extern Pin P8_11;
extern Pin P8_14;
extern Pin P8_12;

// Additional Pin declarations for other pins as needed
extern Pin P9_11;
extern Pin P9_12;
extern Pin P9_13;
extern Pin P9_14;
extern Pin P9_15;
extern Pin P9_16;
extern Pin P9_17;
extern Pin P9_18;
extern Pin P9_21;
extern Pin P9_22;
extern Pin P9_23;
extern Pin P9_24;
extern Pin P9_25;
extern Pin P9_26;
extern Pin P9_27;
extern Pin P9_28;
extern Pin P9_29;
extern Pin P9_30;
extern Pin P9_31;
extern Pin P9_41;
extern Pin P9_42;

#endif // PINS_H

