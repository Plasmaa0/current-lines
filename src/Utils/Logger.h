#pragma once

#include <ctime>
#include <fstream>
#include <ostream>
#include <iostream>
#include <memory>
#include <sstream>
#include <format>
#include <memory>

enum class LogLevel {
    TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL
};

class Logger {
public:
    static void init(const std::string &filename);

    static void init();

    static Logger *getInstance();

    // Logs a message with a given log level
    void log(LogLevel level, const std::string &message);


    Logger(const Logger &) = delete;

    Logger &operator=(Logger &) = delete;

    // Destructor: Closes the log file
    ~Logger() { logFile.close(); }

private:
    Logger(): isInitialized(true), isFile(false) {}

    explicit Logger(const std::string &filename);

private:
    static Logger *instance;
    std::fstream logFile; // File stream for the log file
    bool isInitialized = false;
    bool isFile;

    // Converts log level to a string for output
    static std::string levelToString(LogLevel level);
};

#define LOG_TRACE(...) Logger::getInstance()->log(LogLevel::TRACE, std::format(__VA_ARGS__))
#define LOG_DEBUG(...) Logger::getInstance()->log(LogLevel::DEBUG, std::format(__VA_ARGS__))
#define LOG_INFO(...) Logger::getInstance()->log(LogLevel::INFO, std::format(__VA_ARGS__))
#define LOG_WARNING(...) Logger::getInstance()->log(LogLevel::WARNING, std::format(__VA_ARGS__))
#define LOG_ERROR(...) Logger::getInstance()->log(LogLevel::ERROR, std::format(__VA_ARGS__))
#define LOG_CRITICAL(...) Logger::getInstance()->log(LogLevel::CRITICAL, std::format(__VA_ARGS__))
