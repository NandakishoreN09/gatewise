#include "utilities.h"
#include <iostream>
#include <fstream>
#include <pthread.h> // For thread priority

void writeToSysfs(const std::string &path, const std::string &value) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to write to " << path << std::endl;
        return;
    }
    file << value;
    file.close();
}

void setThreadPriority(std::thread &thread, int priority) {
    struct sched_param sch_params;
    sch_params.sched_priority = priority;
    if (pthread_setschedparam(thread.native_handle(), SCHED_FIFO, &sch_params) != 0) {
        std::cerr << "Failed to set thread priority: " << priority << std::endl;
    }
}

