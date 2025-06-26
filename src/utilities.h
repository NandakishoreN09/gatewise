#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <thread>

// Function to write to sysfs paths
void writeToSysfs(const std::string &path, const std::string &value);

// Function to set thread priority
void setThreadPriority(std::thread &thread, int priority);

#endif // UTILITIES_H

