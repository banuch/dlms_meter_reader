 /**
 * @file HardwareManager.h
 * @brief Hardware control (LEDs, DTR, UART) for DLMS reader
 * @version 2.0
 * @date 2025-10-02
 */

#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <Arduino.h>
#include "../config/pins.h"

/**
 * @enum LEDColor
 * @brief Available LED colors for status indication
 */
enum class LEDColor {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA,
    WHITE,
    OFF
};

/**
 * @class HardwareManager
 * @brief Manages all hardware interfaces
 */
class HardwareManager {
public:
    /**
     * @brief Initialize all hardware
     */
    static void begin();
    
    /**
     * @brief Initialize DLMS UART
     * @param baudRate Baud rate (default 9600)
     */
    static void initDLMSSerial(uint32_t baudRate = DLMS_BAUD_RATE);
    
    // ============================================
    // DTR CONTROL
    // ============================================
    
    /**
     * @brief Wake up meter via DTR
     */
    static void wakeupMeter();
    
    /**
     * @brief Put meter to sleep
     */
    static void sleepMeter();
    
    // ============================================
    // LED CONTROL
    // ============================================
    
    /**
     * @brief Set LED color
     * @param color Color to display
     */
    static void setLED(LEDColor color);
    
    /**
     * @brief Turn all LEDs off
     */
    static void ledsOff();
    
    /**
     * @brief Blink LED pattern
     * @param color Color to blink
     * @param count Number of blinks
     * @param onTime Time LED is on (ms)
     * @param offTime Time LED is off (ms)
     */
    static void blinkLED(LEDColor color, uint8_t count = 1, 
                        uint16_t onTime = 100, uint16_t offTime = 100);
    
    /**
     * @brief Show error pattern
     * @param errorCode Error code (number of blinks)
     */
    static void showError(uint8_t errorCode);
    
    /**
     * @brief Show success pattern
     */
    static void showSuccess();
    
    /**
     * @brief Show activity pulse
     */
    static void showActivity();
    
    /**
     * @brief Show startup sequence
     */
    static void startupSequence();
    
    // ============================================
    // STATUS LED (Built-in)
    // ============================================
    
    /**
     * @brief Turn status LED on
     */
    static void statusLedOn();
    
    /**
     * @brief Turn status LED off
     */
    static void statusLedOff();
    
    /**
     * @brief Toggle status LED
     */
    static void statusLedToggle();
    
    // ============================================
    // UART OPERATIONS
    // ============================================
    
    /**
     * @brief Check if DLMS serial is available
     * @return Number of bytes available
     */
    static int available();
    
    /**
     * @brief Read byte from DLMS serial
     * @return Byte read
     */
    static uint8_t read();
    
    /**
     * @brief Write data to DLMS serial
     * @param data Data buffer
     * @param length Length of data
     * @return Number of bytes written
     */
    static size_t write(const uint8_t* data, size_t length);
    
    /**
     * @brief Flush DLMS serial
     */
    static void flush();
    
    /**
     * @brief Clear DLMS serial receive buffer
     */
    static void clearRxBuffer();
    
private:
    static HardwareSerial* dlmsSerial;
    static bool initialized;
    
    // LED state tracking
    static bool statusLedState;
};

#endif // HARDWARE_MANAGER_H
