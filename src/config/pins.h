 /**
 * @file pins.h
 * @brief Hardware pin definitions for ESP32 DLMS Meter Reader
 * @version 2.0
 * @date 2025-10-02
 */

#ifndef PINS_H
#define PINS_H

// ============================================
// UART PINS - DLMS Communication
// ============================================
#define DLMS_RXD_PIN        16    // ESP32 RX <- Meter TX
#define DLMS_TXD_PIN        17    // ESP32 TX -> Meter RX
#define DLMS_UART_NUM       2     // UART2 for DLMS

// ============================================
// CONTROL PINS
// ============================================
#define DTR_PIN             4     // Data Terminal Ready - Wake up meter
#define RST_PIN             5     // Optional: Meter Reset Pin

// ============================================
// LED INDICATOR PINS
// ============================================
#define STATUS_LED_PIN      2     // Built-in LED (Blue on most ESP32)
#define RED_LED_PIN         12    // Error/Failure indicator
#define GREEN_LED_PIN       13    // Success indicator
#define BLUE_LED_PIN        14    // Activity/Processing indicator

// ============================================
// OPTIONAL I2C PINS (for OLED Display)
// ============================================
#define I2C_SDA_PIN         21    // I2C Data
#define I2C_SCL_PIN         22    // I2C Clock
#define OLED_ENABLED        false // Set to true if using OLED

// ============================================
// OPTIONAL BUTTON PINS
// ============================================
#define RESET_BUTTON_PIN    0     // Boot button (GPIO0)
#define USER_BUTTON_PIN     15    // Optional user button

// ============================================
// UART CONFIGURATION
// ============================================
#define DLMS_BAUD_RATE      9600  // Standard DLMS baud rate
#define DEBUG_BAUD_RATE     115200

// ============================================
// TIMING CONSTANTS
// ============================================
#define DTR_WAKE_DELAY      500   // ms to wait after DTR activation
#define LED_BLINK_DELAY     100   // ms for LED blink duration
#define COMMAND_TIMEOUT     2000  // ms to wait for meter response

#endif // PINS_H
