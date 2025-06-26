#ifndef PARKING_SYSTEM_H
#define PARKING_SYSTEM_H

#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <set>
#include <string>
#include <chrono>
#include "GPIO.h"
#include "OVERLAY.h"
#include "PINS.h"
#include "utilities.h"

// Define constants for gate control and timing
#define GATE_CONTROL_PIN P9_14              // Servo motor pin for gate control
#define GATE_CENTER_DUTY_CYCLE 1500000      // 1.5 ms - neutral position
#define GATE_EXIT_DUTY_CYCLE 2000000        // 2.0 ms - counter clockwise (exit)
#define GATE_ENTRY_DUTY_CYCLE 1000000       // 1.0 ms - clockwise (entry)
#define PWM_PERIOD 20000000                 // 20 ms period (50 Hz)
#define GATE_PASSAGE_DELAY 5000             // 5 seconds delay for car passage
#define SENSOR_DEBOUNCE_DELAY 500           // 500 ms debounce delay

// Define IR sensor and gate sensor pins
#define IR_SENSOR1_PIN Pin{44, "P8_12", gpio, P8_12_modes, 2, none}
#define IR_SENSOR2_PIN Pin{26, "P8_14", gpio, P8_14_modes, 2, none}
#define IR_SENSOR3_PIN Pin{45, "P8_11", gpio, P8_11_modes, 2, none}
#define EXIT_GATE_SENSOR_PIN Pin{47, "P8_15", gpio, P8_15_modes, 2, none}
#define ENTRY_GATE_SENSOR_PIN Pin{46, "P8_16", gpio, P8_16_modes, 2, none}

// Define LED pins with correct GPIO numbers
#define GREEN_LED1_PIN Pin{60, "P9_12", gpio, P9_12_modes, 1, none}   // Spot 1 Green LED
#define RED_LED1_PIN Pin{48, "P9_15", gpio, P9_15_modes, 1, none}     // Spot 1 Red LED
#define GREEN_LED2_PIN Pin{49, "P9_23", gpio, P9_23_modes, 1, none}   // Spot 2 Green LED
#define RED_LED2_PIN Pin{117, "P9_25", gpio, P9_25_modes, 3, none}     // Spot 2 Red LED
#define GREEN_LED3_PIN Pin{115, "P9_27", gpio, P9_27_modes, 3, none}  // Spot 3 Green LED
#define RED_LED3_PIN Pin{20, "P9_41", gpio, P9_41_modes, 3, none}     // Spot 3 Red LED

// Enum to track gate states
enum class GateState {
    CENTERED,    // Gate in neutral position
    OPEN_ENTRY,  // Gate open for entry
    OPEN_EXIT    // Gate open for exit
};

class ParkingSystem {
public:
    ParkingSystem(int totalSpots);
    void initialize();
    void run();
    void stop();

private:
    // System configuration
    int totalSpots;
    std::atomic<int> availableSpots;
    std::atomic<bool> stopFlag;

    // Sensor and control pins
    std::vector<Pin> irSensorPins;           // IR sensors for parking spots
    Pin exitGateSensorPin;                   // IR sensor for detecting exiting cars
    Pin entryGateSensorPin;                  // IR sensor for detecting entering cars
    Pin gateControlPin;                      // Pin for controlling the gate

    // LED pins
    std::vector<Pin> greenLEDPins;           // Green LEDs for available spots
    std::vector<Pin> redLEDPins;             // Red LEDs for occupied spots

    // Synchronization primitives
    std::mutex threadMutex;
    std::mutex displayMutex;
    std::condition_variable cv;

    // State tracking
    std::vector<bool> currentOccupancy;      // Occupancy status of each parking spot
    GateState gateState;                     // Current gate position

    // Helper methods
    void controlGate(GateState newState);
    void updateSpotLEDs(int spotIndex, bool occupied);
    bool initializeLEDPin(Pin& pin, const char* type, int index);

    // Monitoring threads
    void monitorSpots();                     // Monitor parking spot sensors
    void monitorEntryGateSensor();           // Monitor entry gate
    void monitorExitGateSensor();            // Monitor exit gate
    void updateDisplay();                    // Update display with status
    
};

#endif // PARKING_SYSTEM_H
