#include "Logger.h"

Logger *Logger::instance = nullptr;

void Logger::init(const std::string &filename) {
    if (not instance)
        instance = new Logger(filename);
}

void Logger::init() {
    if (not instance)
        instance = new Logger;
}

Logger *Logger::getInstance() {
    if (not instance)
        throw std::runtime_error("tried calling logger without initializing it");
    return instance;
}

void Logger::log(LogLevel level, const std::string &message) {
    // Get current timestamp
    time_t now = time(nullptr);
    tm *timeInfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S", timeInfo);

    // Create log entry
    std::ostringstream logEntry;
    //        logEntry << "[" << timestamp << "] "
    //                 << levelToString(level) << ": " << message
    //                 << std::endl;
    // Output to console
    //        std::cout << logEntry4.str();

    // https://youtrack.jetbrains.com/issue/CPP-33258
    // https://youtrack.jetbrains.com/issue/CPP-38669
    logEntry << std::format("[{}] {}: {}", timestamp, levelToString(level), message) << std::endl;
    if (not isFile) return;
    // Output to log file
    if (logFile.is_open()) {
        logFile << logEntry.str();
        logFile.flush(); // Ensure immediate write to file
    }
}

// Constructor: Opens the log file in append mode
Logger::Logger(const std::string &filename) {
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file." << std::endl;
    }
    isInitialized = true;
    isFile = true;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}
