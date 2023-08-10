#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include "chatDB.h" // getNow()

class Logger {
public:
    Logger(const std::string& logFile) : fileName(logFile) {
        file.open(fileName, std::ios::app); // Open file in append mode
    }

    ~Logger() {
        if (file.is_open()) {
            writeLog("Session closed at" + getNow()); // doesn't work ?????
            file.close();
        }
    }

    void writeLog(const std::string& message);
    std::string readLog();
    std::vector<std::string> getLastNLogs(size_t n);

    std::string getLastLog();

    void initLog();

    std::vector<std::string> getAllLogs();

private:
    std::string fileName;
    std::fstream file;
    mutable std::shared_mutex mutex; // Protects access to the file
};

