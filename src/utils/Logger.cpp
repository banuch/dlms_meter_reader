 /**
 * @file Logger.cpp
 * @brief Implementation of logging system
 * @version 2.0
 * @date 2025-10-02
 */

#include "Logger.h"

// Initialize static members
Logger::Level Logger::currentLevel = Logger::INFO;
bool Logger::colorsEnabled = true;
bool Logger::timestampEnabled = true;

/**
 * @brief Initialize the logger system
 */
void Logger::begin(Level level) {
    currentLevel = level;
    Serial.println("\n========================================");
    Serial.println("  DLMS Meter Reader v" FIRMWARE_VERSION);
    Serial.println("  Logger Initialized");
    Serial.println("========================================\n");
}

/**
 * @brief Main logging function
 */
void Logger::log(Level level, const char* message) {
    if (level > currentLevel) return;
    
    String output = "";
    
    // Add timestamp if enabled
    if (timestampEnabled) {
        output += "[" + getTimestamp() + "] ";
    }
    
    // Add colored level indicator
    if (colorsEnabled) {
        output += getLevelColor(level);
    }
    
    output += "[" + String(getLevelString(level)) + "] ";
    
    if (colorsEnabled) {
        output += COLOR_RESET;
    }
    
    output += message;
    
    Serial.println(output);
}

/**
 * @brief Error level logging
 */
void Logger::error(const char* message) {
    log(ERROR, message);
}

void Logger::error(const String& message) {
    error(message.c_str());
}

/**
 * @brief Warning level logging
 */
void Logger::warn(const char* message) {
    log(WARN, message);
}

void Logger::warn(const String& message) {
    warn(message.c_str());
}

/**
 * @brief Info level logging
 */
void Logger::info(const char* message) {
    log(INFO, message);
}

void Logger::info(const String& message) {
    info(message.c_str());
}

/**
 * @brief Debug level logging
 */
void Logger::debug(const char* message) {
    log(DEBUG, message);
}

void Logger::debug(const String& message) {
    debug(message.c_str());
}

/**
 * @brief Print hexadecimal dump of data
 */
void Logger::hexDump(const char* label, const uint8_t* data, 
                     uint16_t length, Level level) {
    if (level > currentLevel) return;
    
    String output = String(label) + " [" + String(length) + " bytes]: ";
    
    for (uint16_t i = 0; i < length; i++) {
        if (data[i] < 0x10) output += "0";
        output += String(data[i], HEX);
        output += " ";
        
        // Add line break every 16 bytes for readability
        if ((i + 1) % 16 == 0 && i < length - 1) {
            output += "\n                           ";
        }
    }
    
    log(level, output.c_str());
}

/**
 * @brief Get string representation of log level
 */
const char* Logger::getLevelString(Level level) {
    switch (level) {
        case ERROR: return "ERROR";
        case WARN:  return "WARN ";
        case INFO:  return "INFO ";
        case DEBUG: return "DEBUG";
        default:    return "?????";
    }
}

/**
 * @brief Get ANSI color code for log level
 */
const char* Logger::getLevelColor(Level level) {
    switch (level) {
        case ERROR: return COLOR_RED;
        case WARN:  return COLOR_YELLOW;
        case INFO:  return COLOR_GREEN;
        case DEBUG: return COLOR_CYAN;
        default:    return COLOR_RESET;
    }
}

/**
 * @brief Get current timestamp string
 */
String Logger::getTimestamp() {
    unsigned long ms = millis();
    unsigned long seconds = ms / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    seconds %= 60;
    minutes %= 60;
    ms %= 1000;
    
    char buffer[16];
    sprintf(buffer, "%02lu:%02lu:%02lu.%03lu", hours, minutes, seconds, ms);
    return String(buffer);
}

/**
 * @brief Set minimum log level
 */
void Logger::setLevel(Level level) {
    currentLevel = level;
}

/**
 * @brief Enable or disable colored output
 */
void Logger::enableColors(bool enable) {
    colorsEnabled = enable;
}

/**
 * @brief Enable or disable timestamps
 */
void Logger::enableTimestamp(bool enable) {
    timestampEnabled = enable;
}
