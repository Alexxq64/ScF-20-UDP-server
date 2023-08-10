#include "logger.h"

    void Logger::writeLog(const std::string& message) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        if (file.is_open()) {
            file << message << std::endl;
        }
    }

    std::string Logger::readLog() {
        std::shared_lock<std::shared_mutex> lock(mutex);
        std::string log;
        if (file.is_open() && std::getline(file, log)) {
            return log;
        }
        return "";
    }



    std::vector<std::string> Logger::getLastNLogs(size_t n) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        std::ifstream file(fileName);
        std::vector<std::string> logs;
        std::string log;

        while (std::getline(file, log)) {
            logs.push_back(log);
            if (logs.size() > n) {
                logs.erase(logs.begin());
            }
        }

        return logs;
    }

    std::string Logger::getLastLog() {
        return Logger::getLastNLogs(1)[0];
    }

// doesn't work
    void Logger::initLog() {
        std::unique_lock<std::shared_mutex> lock(mutex);
        file.open(fileName, std::ios::trunc);
        if (file.is_open()) {
            file << "Session opened at " << getNow() << std::endl;
        }
    }

    std::vector<std::string> Logger::getAllLogs() {
        std::unique_lock<std::shared_mutex> lock(mutex);
        std::ifstream file(fileName);
        std::vector<std::string> logs;
        std::string log;

        while (std::getline(file, log)) {
            logs.push_back(log);
        }

        return logs;
    }