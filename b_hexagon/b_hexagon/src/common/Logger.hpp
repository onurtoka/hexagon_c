/**
 * @file Logger.hpp
 * @brief Simple logging utility for B_Hexagon ap    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_as_time_t = std::chrono::system_clock::to_time_t(now);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_as_time_t), "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
        return oss.str();
    }*/

#pragma once

#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <string>

/**
 * @brief Simple logging levels
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

/**
 * @brief Simple logger class for application status tracking
 */
class Logger {
public:
    /**
     * @brief Set the minimum log level to display
     */
    static void setLogLevel(LogLevel level) {
        minLevel_ = level;
    }

    /**
     * @brief Log a debug message
     */
    template<typename... Args>
    static void debug(Args&&... args) {
        log(LogLevel::DEBUG, "DEBUG", std::forward<Args>(args)...);
    }

    /**
     * @brief Log an info message
     */
    template<typename... Args>
    static void info(Args&&... args) {
        log(LogLevel::INFO, "INFO ", std::forward<Args>(args)...);
    }

    /**
     * @brief Log a warning message
     */
    template<typename... Args>
    static void warn(Args&&... args) {
        log(LogLevel::WARN, "WARN ", std::forward<Args>(args)...);
    }

    /**
     * @brief Log an error message
     */
    template<typename... Args>
    static void error(Args&&... args) {
        log(LogLevel::ERROR, "ERROR", std::forward<Args>(args)...);
    }

private:
    static LogLevel minLevel_;

    /**
     * @brief Get current timestamp string
     */
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    /**
     * @brief Internal logging function
     */
    template<typename... Args>
    static void log(LogLevel level, const char* levelStr, Args&&... args) {
        if (level < minLevel_) {
            return;
        }

        std::cout << "[" << getCurrentTimestamp() << "] [" << levelStr << "] ";
        ((std::cout << std::forward<Args>(args)), ...);
        std::cout << std::endl;
    }
};

// Static member definition
inline LogLevel Logger::minLevel_ = LogLevel::INFO;
