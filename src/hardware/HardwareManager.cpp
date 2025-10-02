 /**
 * @file HardwareManager.cpp
 * @brief Implementation of hardware control
 * @version 2.0
 * @date 2025-10-02
 */

#include "HardwareManager.h"

// Initialize static members
HardwareSerial* HardwareManager::dlmsSerial = nullptr;
bool HardwareManager::initialized = false;
bool HardwareManager::statusLedState = false;

/**
 * @brief Initialize all hardware components
 */
void HardwareManager::begin() {
    if (initialized) return;
    
    // Initialize LED pins
    pinMode(STATUS_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    
    // Initialize control pins
    pinMode(DTR_PIN, OUTPUT);
    
    // Start with all LEDs off
    ledsOff();
    statusLedOff();
    
    // DTR low (meter awake by default)
    digitalWrite(DTR_PIN, LOW);
    
    // Initialize DLMS serial
    initDLMSSerial();
    
    initialized = true;
    
    // Show startup sequence
    startupSequence();
}

/**
 * @brief Initialize DLMS UART
 */
void HardwareManager::initDLMSSerial(uint32_t baudRate) {
    dlmsSerial = &Serial2;
    dlmsSerial->begin(baudRate, SERIAL_8N1, DLMS_RXD_PIN, DLMS_TXD_PIN);
    dlmsSerial->setTimeout(1000);
}

// ============================================
// DTR CONTROL
// ============================================

void HardwareManager::wakeupMeter() {
    digitalWrite(DTR_PIN, LOW);
    delay(DTR_WAKE_DELAY);
}

void HardwareManager::sleepMeter() {
    digitalWrite(DTR_PIN, HIGH);
    delay(100);
}

// ============================================
// LED CONTROL
// ============================================

void HardwareManager::setLED(LEDColor color) {
    // Active LOW LEDs (common anode RGB)
    switch (color) {
        case LEDColor::RED:
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, HIGH);
            digitalWrite(BLUE_LED_PIN, HIGH);
            break;
        case LEDColor::GREEN:
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(GREEN_LED_PIN, LOW);
            digitalWrite(BLUE_LED_PIN, HIGH);
            break;
        case LEDColor::BLUE:
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(GREEN_LED_PIN, HIGH);
            digitalWrite(BLUE_LED_PIN, LOW);
            break;
        case LEDColor::YELLOW:
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, LOW);
            digitalWrite(BLUE_LED_PIN, HIGH);
            break;
        case LEDColor::CYAN:
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(GREEN_LED_PIN, LOW);
            digitalWrite(BLUE_LED_PIN, LOW);
            break;
        case LEDColor::MAGENTA:
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, HIGH);
            digitalWrite(BLUE_LED_PIN, LOW);
            break;
        case LEDColor::WHITE:
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, LOW);
            digitalWrite(BLUE_LED_PIN, LOW);
            break;
        case LEDColor::OFF:
        default:
            ledsOff();
            break;
    }
}

void HardwareManager::ledsOff() {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(BLUE_LED_PIN, HIGH);
}

void HardwareManager::blinkLED(LEDColor color, uint8_t count, 
                               uint16_t onTime, uint16_t offTime) {
    for (uint8_t i = 0; i < count; i++) {
        setLED(color);
        delay(onTime);
        ledsOff();
        if (i < count - 1) {
            delay(offTime);
        }
    }
}

void HardwareManager::showError(uint8_t errorCode) {
    blinkLED(LEDColor::RED, errorCode, 200, 200);
    delay(500);
}

void HardwareManager::showSuccess() {
    setLED(LEDColor::GREEN);
    delay(1000);
    ledsOff();
}

void HardwareManager::showActivity() {
    blinkLED(LEDColor::BLUE, 1, 50, 0);
}

void HardwareManager::startupSequence() {
    blinkLED(LEDColor::RED, 1, 100, 100);
    blinkLED(LEDColor::GREEN, 1, 100, 100);
    blinkLED(LEDColor::BLUE, 1, 100, 100);
    delay(200);
}

// ============================================
// STATUS LED
// ============================================

void HardwareManager::statusLedOn() {
    digitalWrite(STATUS_LED_PIN, HIGH);
    statusLedState = true;
}

void HardwareManager::statusLedOff() {
    digitalWrite(STATUS_LED_PIN, LOW);
    statusLedState = false;
}

void HardwareManager::statusLedToggle() {
    statusLedState = !statusLedState;
    digitalWrite(STATUS_LED_PIN, statusLedState);
}

// ============================================
// UART OPERATIONS
// ============================================

int HardwareManager::available() {
    return dlmsSerial ? dlmsSerial->available() : 0;
}

uint8_t HardwareManager::read() {
    return dlmsSerial ? dlmsSerial->read() : 0;
}

size_t HardwareManager::write(const uint8_t* data, size_t length) {
    if (!dlmsSerial) return 0;
    return dlmsSerial->write(data, length);
}

void HardwareManager::flush() {
    if (dlmsSerial) {
        dlmsSerial->flush();
    }
}

void HardwareManager::clearRxBuffer() {
    if (!dlmsSerial) return;
    while (dlmsSerial->available()) {
        dlmsSerial->read();
    }
}
