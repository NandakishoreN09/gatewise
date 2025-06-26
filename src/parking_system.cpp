#include "parking_system.h"
#include "utilities.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

ParkingSystem::ParkingSystem(int totalSpots)
    : totalSpots(totalSpots), 
      availableSpots(totalSpots), 
      stopFlag(false),
      currentOccupancy(totalSpots, false),
      gateState(GateState::CENTERED) {
    // Initialize IR sensor pins using definitions from header
    irSensorPins.push_back(IR_SENSOR1_PIN);
    irSensorPins.push_back(IR_SENSOR2_PIN);
    irSensorPins.push_back(IR_SENSOR3_PIN);

    // Initialize gate sensor pins using definitions from header
    exitGateSensorPin = EXIT_GATE_SENSOR_PIN;
    entryGateSensorPin = ENTRY_GATE_SENSOR_PIN;
    gateControlPin = GATE_CONTROL_PIN;

    // Initialize LED pins
    greenLEDPins.push_back(GREEN_LED1_PIN);
    greenLEDPins.push_back(GREEN_LED2_PIN);
    greenLEDPins.push_back(GREEN_LED3_PIN);

    redLEDPins.push_back(RED_LED1_PIN);
    redLEDPins.push_back(RED_LED2_PIN);
    redLEDPins.push_back(RED_LED3_PIN);
}

bool ParkingSystem::initializeLEDPin(Pin& pin, const char* type, int index) {
    std::string exportPath = "/sys/class/gpio/export";
    std::string gpioPath = "/sys/class/gpio/gpio" + std::to_string(pin.pinNum);

    // Check if the pin is already exported
    if (access(gpioPath.c_str(), F_OK) != 0) {
        // Export the pin
        std::ofstream exportFile(exportPath);
        if (exportFile.is_open()) {
            exportFile << pin.pinNum;
            exportFile.close();
            usleep(100000); // Wait for export to complete
        } else {
            std::cerr << "Failed to export GPIO pin " << pin.pinNum << " for " << type << " LED " << index + 1 << std::endl;
            return false;
        }
    }

    // Set direction to OUTPUT
    std::ofstream directionFile(gpioPath + "/direction");
    if (directionFile.is_open()) {
        directionFile << "out";
        directionFile.close();
    } else {
        std::cerr << "Failed to set direction for GPIO pin " << pin.pinNum << " (" << type << " LED " << index + 1 << ")" << std::endl;
        return false;
    }

    // Set initial state to LOW
    std::ofstream valueFile(gpioPath + "/value");
    if (valueFile.is_open()) {
        valueFile << "0";
        valueFile.close();
    } else {
        std::cerr << "Failed to set initial state for GPIO pin " << pin.pinNum << " (" << type << " LED " << index + 1 << ")" << std::endl;
        return false;
    }

    std::cout << type << " LED " << index + 1 << " initialized successfully" << std::endl;
    return true;
}

void ParkingSystem::updateSpotLEDs(int spotIndex, bool occupied) {
    if (spotIndex < 0 || static_cast<std::vector<Pin>::size_type>(spotIndex) >= greenLEDPins.size()) return;
    
    if (digitalWrite(greenLEDPins[spotIndex], occupied ? LOW : HIGH) < 0) {
        std::cerr << "Failed to control green LED " << spotIndex + 1 << std::endl;
    }
    if (digitalWrite(redLEDPins[spotIndex], occupied ? HIGH : LOW) < 0) {
        std::cerr << "Failed to control red LED " << spotIndex + 1 << std::endl;
    }
}

void ParkingSystem::initialize() {
    std::cout << "Initializing Parking System..." << std::endl;

    if (!gpioInstance()) {
        std::cerr << "Critical Error: Failed to initialize GPIO instance!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize IR sensors for spots
    for (auto& pin : irSensorPins) {
        unexport_gpio(pin.pinNum);
        pin.selectedMode = gpio;
        if (pinMode(pin, INPUT) < 0) {
            std::cerr << "Failed to configure pin " << pin.pinNum << " as INPUT" << std::endl;
            continue;
        }
        std::cout << "GPIO pin " << pin.pinNum << " configured as INPUT" << std::endl;
    }

    // Initialize gate sensors
    unexport_gpio(exitGateSensorPin.pinNum);
    unexport_gpio(entryGateSensorPin.pinNum);
    
    if (pinMode(exitGateSensorPin, INPUT) < 0 || pinMode(entryGateSensorPin, INPUT) < 0) {
        std::cerr << "Failed to configure gate sensor pins" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize LED pins
    std::cout << "Initializing LED pins..." << std::endl;
    for (size_t i = 0; i < greenLEDPins.size(); ++i) {
        bool green_ok = initializeLEDPin(greenLEDPins[i], "Green", i);
        bool red_ok = initializeLEDPin(redLEDPins[i], "Red", i);
        
        if (green_ok && red_ok) {
            updateSpotLEDs(i, false);  // Initialize to available state
        }
    }

    // Initialize servo motor (PWM)
    std::string pwmExportPath = "/sys/class/pwm/pwmchip4/export";
    std::string pwmPeriodPath = "/sys/class/pwm/pwmchip4/pwm-4:0/period";
    std::string pwmDutyCyclePath = "/sys/class/pwm/pwmchip4/pwm-4:0/duty_cycle";
    std::string pwmEnablePath = "/sys/class/pwm/pwmchip4/pwm-4:0/enable";

    if (access("/sys/class/pwm/pwmchip4/pwm-4:0", F_OK) != 0) {
        writeToSysfs(pwmExportPath, "0");
    }

    writeToSysfs(pwmPeriodPath, std::to_string(PWM_PERIOD));
    writeToSysfs(pwmDutyCyclePath, std::to_string(GATE_CENTER_DUTY_CYCLE));
    writeToSysfs(pwmEnablePath, "1");

    std::cout << "Centering gate at startup..." << std::endl;
    controlGate(GateState::CENTERED);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "System initialized with " << totalSpots << " parking spots." << std::endl;
}

void ParkingSystem::controlGate(GateState newState) {
    std::string pwmDutyCyclePath = "/sys/class/pwm/pwmchip4/pwm-4:0/duty_cycle";
    
    int dutyCycle;
    switch(newState) {
        case GateState::OPEN_ENTRY:
            std::cout << "Opening gate for entry (clockwise)..." << std::endl;
            dutyCycle = GATE_ENTRY_DUTY_CYCLE;
            break;
        case GateState::OPEN_EXIT:
            std::cout << "Opening gate for exit (counter-clockwise)..." << std::endl;
            dutyCycle = GATE_EXIT_DUTY_CYCLE;
            break;
        case GateState::CENTERED:
            std::cout << "Centering gate..." << std::endl;
            dutyCycle = GATE_CENTER_DUTY_CYCLE;
            break;
    }

    writeToSysfs(pwmDutyCyclePath, std::to_string(dutyCycle));
    gateState = newState;
}

void ParkingSystem::run() {
    std::thread spotThread(&ParkingSystem::monitorSpots, this);
    std::thread exitThread(&ParkingSystem::monitorExitGateSensor, this);
    std::thread entryThread(&ParkingSystem::monitorEntryGateSensor, this);
    std::thread displayThread(&ParkingSystem::updateDisplay, this);

    setThreadPriority(entryThread, 80); // High priority for entry sensor
    setThreadPriority(exitThread, 80); // High priority for exit sensor
    setThreadPriority(spotThread, 50); // Medium priority for spot monitoring
    setThreadPriority(displayThread, 30); // Low priority for display updates

    spotThread.detach();
    exitThread.detach();
    entryThread.detach();
    displayThread.detach();
}

void ParkingSystem::stop() {
    {
        std::lock_guard<std::mutex> lock(threadMutex);
        stopFlag = true;
    }
    cv.notify_all();

    for (size_t i = 0; i < greenLEDPins.size(); ++i) {
        digitalWrite(greenLEDPins[i], LOW);
        digitalWrite(redLEDPins[i], LOW);
    }

    controlGate(GateState::CENTERED);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    writeToSysfs("/sys/class/pwm/pwmchip4/pwm-4:0/enable", "0");
}

void ParkingSystem::monitorSpots() {
    while (!stopFlag) {
        for (size_t i = 0; i < irSensorPins.size(); ++i) {
            int status = digitalRead(irSensorPins[i]);
            if (status >= 0) {
                std::lock_guard<std::mutex> lock(displayMutex);
                bool occupied = (status == LOW);
                if (occupied != currentOccupancy[i]) {
                    currentOccupancy[i] = occupied;
                    updateSpotLEDs(i, occupied);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void ParkingSystem::monitorEntryGateSensor() {
    bool lastSensorState = HIGH;
    std::chrono::steady_clock::time_point lastChange;

    while (!stopFlag) {
        int sensorStatus = digitalRead(entryGateSensorPin);
        if (sensorStatus < 0) continue;

        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastChange);

        if (duration.count() >= SENSOR_DEBOUNCE_DELAY) {
            std::lock_guard<std::mutex> lock(displayMutex);

            if (sensorStatus == LOW && lastSensorState == HIGH) {
                // Car detected at entry
                if (availableSpots > 0) {
                    controlGate(GateState::OPEN_ENTRY);
                    // Decrement availability after car passes (with delay)
                    std::this_thread::sleep_for(std::chrono::milliseconds(GATE_PASSAGE_DELAY));
                    availableSpots--;
                    controlGate(GateState::CENTERED);
                }
            }

            lastSensorState = sensorStatus;
            lastChange = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void ParkingSystem::monitorExitGateSensor() {
    bool lastSensorState = HIGH;
    std::chrono::steady_clock::time_point lastChange;

    while (!stopFlag) {
        int sensorStatus = digitalRead(exitGateSensorPin);
        if (sensorStatus < 0) continue;

        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastChange);

        if (duration.count() >= SENSOR_DEBOUNCE_DELAY) {
            std::lock_guard<std::mutex> lock(displayMutex);

            if (sensorStatus == LOW && lastSensorState == HIGH) {
                // Car detected at exit
                controlGate(GateState::OPEN_EXIT);
                // Increment availability after car passes (with delay)
                std::this_thread::sleep_for(std::chrono::milliseconds(GATE_PASSAGE_DELAY));
                if (availableSpots < totalSpots) {
                    availableSpots++;
                }
                controlGate(GateState::CENTERED);
            }

            lastSensorState = sensorStatus;
            lastChange = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void ParkingSystem::updateDisplay() {
    int lastAvailable = -1;
    GateState lastGateState = GateState::CENTERED;

    while (!stopFlag) {
        std::lock_guard<std::mutex> lock(displayMutex);
        
        if (availableSpots != lastAvailable || gateState != lastGateState) {
            std::cout << "Parking Status - Available: " << availableSpots 
                      << "/" << totalSpots 
                      << " (Occupied: " << (totalSpots - availableSpots) << ")"
                      << " [Gate: " << (gateState == GateState::CENTERED ? "CENTERED" :
                                      gateState == GateState::OPEN_ENTRY ? "ENTRY" : "EXIT")
                      << "]" << std::endl;

            lastAvailable = availableSpots;
            lastGateState = gateState;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
