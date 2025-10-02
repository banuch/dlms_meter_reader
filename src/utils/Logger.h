 /**
 * @file Logger.h
 * @brief Multi-level logging system with timestamp and color support
 * @version 2.0
 * @date 2025-10-02
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "../config/config.h"

// ANSI Color codes for serial terminal
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

/**
 * @class Logger
 * @brief Provides structured logging with different severity levels
 */
class Logger {
public:
    enum Level {
        ERROR = 0,
        WARN = 1,
        INFO = 2,
        DEBUG = 3
    };
    
    /**
     * @brief Initialize the logger
     * @param level Minimum log level to display
     */
    static void begin(Level level = INFO);
    
    /**
     * @brief Log error message
     * @param message Message to log
     */
    static void error(const char* message);
    static void error(const String& message);
    
    /**
     * @brief Log warning message
     * @param message Message to log
     */
    static void warn(const char* message);
    static void warn(const String& message);
    
    /**
     * @brief Log info message
     * @param message Message to log
     */
    static void info(const char* message);
    static void info(const String& message);
    
    /**
     * @brief Log debug message
     * @param message Message to log
     */
    static void debug(const char* message);
    static void debug(const String& message);
    
    /**
     * @brief Print hex dump of data buffer
     * @param label Label for the data
     * @param data Pointer to data
     * @param length Length of data
     * @param level Log level for this dump
     */
    static void hexDump(const char* label, const uint8_t* data, 
                       uint16_t length, Level level = DEBUG);
    
    /**
     * @brief Set minimum log level
     * @param level New minimum level
     */
    static void setLevel(Level level);
    
    /**
     * @brief Enable/disable colored output
     * @param enable true to enable colors
     */
    static void enableColors(bool enable);
    
    /**
     * @brief Enable/disable timestamps
     * @param enable true to enable timestamps
     */
    static void enableTimestamp(bool enable);

private:
    static Level currentLevel;
    static bool colorsEnabled;
    static bool timestampEnabled;
    
    static void log(Level level, const char* message);
    static const char* getLevelString(Level level);
    static const char* getLevelColor(Level level);
    static String getTimestamp();
};

// Convenience macros for conditional logging
#if DEBUG_MODE
    #define LOG_ERROR(msg)   Logger::error(msg)
    #define LOG_WARN(msg)    Logger::warn(msg)
    #define LOG_INFO(msg)    Logger::info(msg)
    #define LOG_DEBUG(msg)   Logger::debug(msg)
    #define LOG_HEX(label, data, len) Logger::hexDump(label, data, len)
#else
    #define LOG_ERROR(msg)   Logger::error(msg)
    #define LOG_WARN(msg)    Logger::warn(msg)
    #define LOG_INFO(msg)    ((void)0)
    #define LOG_DEBUG(msg)   ((void)0)
    #define LOG_HEX(label, data, len) ((void)0)
#endif

#endif // LOGGER_H
