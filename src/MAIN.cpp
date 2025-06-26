/*
MAIN.cpp
Implementation of the main logic for the parking system
*/

#include "Wiring.h"
#include "parking_system.h"
#include "MAIN.h"
#include <signal.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <exception>

// Global atomic flag to manage program state
std::atomic<bool> running(true);
std::atomic<bool> setupComplete(false);

// Signal handler function to stop the program
void StopProgram(int signal) {
    if (setupComplete) {
        std::cout << "\nSignal " << signal << " received. Stopping program..." << std::endl;
    }
    running = false;  // Set the flag to stop the loop
}

// Parking system instance (initialize with 3 parking spots)
ParkingSystem parkingSystem(3);

// User-defined setup function
void setup() {
    try {
        std::cout << "Setting up the parking system..." << std::endl;
        parkingSystem.initialize();  // Initialize the parking system
        std::cout << "Setup completed successfully. All 3 parking spots are available." << std::endl;
        std::cout << "Gate initialized in centered position." << std::endl;
        setupComplete = true;
    } catch (const std::exception& e) {
        std::cerr << "Critical Error during setup: " << e.what() << std::endl;
        running = false;  // Stop the program if initialization fails
    } catch (...) {
        std::cerr << "Critical Unknown error occurred during setup." << std::endl;
        running = false;
    }
}

// User-defined loop function
void loop() {
    static bool firstRun = true;
    
    try {
        if (!running) return;       // Exit the loop if the program is stopping
        
        if (firstRun) {
            parkingSystem.run();    // Only start the monitoring threads once
            firstRun = false;
        }
        
        // Sleep to prevent tight looping
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } catch (const std::exception& e) {
        std::cerr << "Error in loop: " << e.what() << std::endl;
        running = false;  // Stop the program on exception
    } catch (...) {
        std::cerr << "Unknown error occurred in loop." << std::endl;
        running = false;
    }
}

int main() {
    // Register signal handlers
    signal(SIGINT, StopProgram);    // Ctrl+C
    signal(SIGTERM, StopProgram);   // Termination request

    std::cout << "\nStarting Bi-Directional Parking Gate System...\n" << std::endl;

    // Call user-defined setup function
    setup();

    // Main loop
    while (running) {
        loop();
    }

    // Stop the parking system when exiting
    if (setupComplete) {
        try {
            parkingSystem.stop();
            std::cout << "Parking system stopped successfully." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error while stopping the parking system: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error occurred while stopping the parking system." << std::endl;
        }
        std::cout << "Program terminated gracefully." << std::endl;
    }
    
    return 0;
}


