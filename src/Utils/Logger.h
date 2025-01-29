#pragma once

#include <ctime>
#include <fstream>
#include <ostream>
#include <iostream>
#include <memory>
#include <format>

enum class LogLevel : uint8_t {
    TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL
};

inline auto operator<=>(const LogLevel left, const LogLevel right) {
    return static_cast<uint8_t>(left) <=> static_cast<uint8_t>(right);
}

class Logger {
public:
    static void init(const std::string &filename);

    static void init();

    static void setLogLevel(LogLevel level);

    static Logger *getInstance();

    // Logs a message with a given log level
    void log(LogLevel level, const std::string &message, const std::string &from = "");


    Logger(const Logger &) = delete;

    Logger &operator=(Logger &) = delete;

    // Destructor: Closes the log file
    ~Logger() { logFile.close(); }

private:
    Logger(): isInitialized(true), isFile(false) {
    }

    explicit Logger(const std::string &filename);

private:
    static Logger *instance;
    std::fstream logFile; // File stream for the log file
    bool isInitialized = false;
    bool isFile;
    LogLevel logLevel;

    // Converts log level to a string for output
    static std::string levelToString(LogLevel level);
};

#if 1
#define LOG_TRACE(...) Logger::getInstance()->log(LogLevel::TRACE, std::format(__VA_ARGS__), __func__)
#define LOG_DEBUG(...) Logger::getInstance()->log(LogLevel::DEBUG, std::format(__VA_ARGS__), __func__)
#define LOG_INFO(...) Logger::getInstance()->log(LogLevel::INFO, std::format(__VA_ARGS__), __func__)
#define LOG_WARNING(...) Logger::getInstance()->log(LogLevel::WARNING, std::format(__VA_ARGS__), __func__)
#define LOG_ERROR(...) Logger::getInstance()->log(LogLevel::ERROR, std::format(__VA_ARGS__), __func__)
#define LOG_CRITICAL(...) Logger::getInstance()->log(LogLevel::CRITICAL, std::format(__VA_ARGS__), __func__)
#else
#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#define LOG_CRITICAL(...)
#endif
