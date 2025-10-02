 /**
 * @file OBISCodes.cpp
 * @brief Implementation of OBIS code definitions
 * @version 2.0
 * @date 2025-10-02
 */

#include "OBISCodes.h"

// ============================================
// METER IDENTIFICATION
// ============================================
const OBISCode OBISCodes::METER_SERIAL_NUMBER(0x00, 0x00, 0x60, 0x01, 0x00, 0xFF, 
    "Serial Number", "", 0x01);
const OBISCode OBISCodes::METER_MANUFACTURER(0x00, 0x00, 0x60, 0x01, 0x01, 0xFF,
    "Manufacturer", "", 0x01);
const OBISCode OBISCodes::METER_TYPE(0x00, 0x00, 0x60, 0x01, 0x02, 0xFF,
    "Meter Type", "", 0x01);

// ============================================
// ENERGY - ACTIVE IMPORT/EXPORT
// ============================================
const OBISCode OBISCodes::KWH_IMPORT(0x01, 0x00, 0x01, 0x08, 0x00, 0xFF,
    "Active Energy Import", "kWh", 0x03);
const OBISCode OBISCodes::KWH_EXPORT(0x01, 0x00, 0x02, 0x08, 0x00, 0xFF,
    "Active Energy Export", "kWh", 0x03);

// TOD Rates for Import
const OBISCode OBISCodes::KWH_IMPORT_RATE[8] = {
    OBISCode(0x01, 0x00, 0x01, 0x08, 0x01, 0xFF, "kWh Import Rate 1", "kWh", 0x03),
    OBISCode(0x01, 0x00, 0x01, 0x08, 0x02, 0xFF, "kWh Import Rate 2", "kWh", 0x03),
    OBISCode(0x01, 0x00, 0x01, 0x08, 0x03, 0xFF, "kWh Import Rate 3", "kWh", 0x03),
    OBISCode(0x01, 0x00, 0x01, 0x08, 0x04, 0xFF, "kWh Import Rate 4", "kWh", 0x03),
    OBISCode(0x01, 0x00, 0x01, 0x08, 0x05, 0xFF, "kWh Import Rate 5", "kWh", 0x03),
    OBISCode(0x01, 0x00, 0x01, 0x08, 0x06, 0xFF, "kWh Import Rate 6", "kWh", 0x03),
    OBISCode(0x01, 0x00, 0x01, 0x08, 0x07, 0xFF, "kWh Import Rate 7", "kWh", 0x03),
    OBISCode(0x01, 0x00, 0x01, 0x08, 0x08, 0xFF, "kWh Import Rate 8", "kWh", 0x03)
};

// ============================================
// ENERGY - APPARENT
// ============================================
const OBISCode OBISCodes::KVAH_IMPORT(0x01, 0x00, 0x09, 0x08, 0x00, 0xFF,
    "Apparent Energy Import", "kVAh", 0x03);
const OBISCode OBISCodes::KVAH_EXPORT(0x01, 0x00, 0x10, 0x08, 0x00, 0xFF,
    "Apparent Energy Export", "kVAh", 0x03);

const OBISCode OBISCodes::KVAH_IMPORT_RATE[8] = {
    OBISCode(0x01, 0x00, 0x09, 0x08, 0x01, 0xFF, "kVAh Import Rate 1", "kVAh", 0x03),
    OBISCode(0x01, 0x00, 0x09, 0x08, 0x02, 0xFF, "kVAh Import Rate 2", "kVAh", 0x03),
    OBISCode(0x01, 0x00, 0x09, 0x08, 0x03, 0xFF, "kVAh Import Rate 3", "kVAh", 0x03),
    OBISCode(0x01, 0x00, 0x09, 0x08, 0x04, 0xFF, "kVAh Import Rate 4", "kVAh", 0x03),
    OBISCode(0x01, 0x00, 0x09, 0x08, 0x05, 0xFF, "kVAh Import Rate 5", "kVAh", 0x03),
    OBISCode(0x01, 0x00, 0x09, 0x08, 0x06, 0xFF, "kVAh Import Rate 6", "kVAh", 0x03),
    OBISCode(0x01, 0x00, 0x09, 0x08, 0x07, 0xFF, "kVAh Import Rate 7", "kVAh", 0x03),
    OBISCode(0x01, 0x00, 0x09, 0x08, 0x08, 0xFF, "kVAh Import Rate 8", "kVAh", 0x03)
};

// ============================================
// ENERGY - REACTIVE
// ============================================
const OBISCode OBISCodes::KVARH_LAG(0x01, 0x00, 0x05, 0x08, 0x00, 0xFF,
    "Reactive Energy Lag", "kVArh", 0x03);
const OBISCode OBISCodes::KVARH_LEAD(0x01, 0x00, 0x08, 0x08, 0x00, 0xFF,
    "Reactive Energy Lead", "kVArh", 0x03);

// ============================================
// MAXIMUM DEMAND - ACTIVE
// ============================================
const OBISCode OBISCodes::MD_KW_IMPORT(0x01, 0x00, 0x01, 0x06, 0x00, 0xFF,
    "MD Active Import", "kW", 0x04);
const OBISCode OBISCodes::MD_KW_EXPORT(0x01, 0x00, 0x02, 0x06, 0x00, 0xFF,
    "MD Active Export", "kW", 0x04);

const OBISCode OBISCodes::MD_KW_IMPORT_RATE[8] = {
    OBISCode(0x01, 0x00, 0x01, 0x06, 0x01, 0xFF, "MD kW Import Rate 1", "kW", 0x04),
    OBISCode(0x01, 0x00, 0x01, 0x06, 0x02, 0xFF, "MD kW Import Rate 2", "kW", 0x04),
    OBISCode(0x01, 0x00, 0x01, 0x06, 0x03, 0xFF, "MD kW Import Rate 3", "kW", 0x04),
    OBISCode(0x01, 0x00, 0x01, 0x06, 0x04, 0xFF, "MD kW Import Rate 4", "kW", 0x04),
    OBISCode(0x01, 0x00, 0x01, 0x06, 0x05, 0xFF, "MD kW Import Rate 5", "kW", 0x04),
    OBISCode(0x01, 0x00, 0x01, 0x06, 0x06, 0xFF, "MD kW Import Rate 6", "kW", 0x04),
    OBISCode(0x01, 0x00, 0x01, 0x06, 0x07, 0xFF, "MD kW Import Rate 7", "kW", 0x04),
    OBISCode(0x01, 0x00, 0x01, 0x06, 0x08, 0xFF, "MD kW Import Rate 8", "kW", 0x04)
};

// ============================================
// MAXIMUM DEMAND - APPARENT
// ============================================
const OBISCode OBISCodes::MD_KVA_IMPORT(0x01, 0x00, 0x09, 0x06, 0x00, 0xFF,
    "MD Apparent Import", "kVA", 0x04);
const OBISCode OBISCodes::MD_KVA_EXPORT(0x01, 0x00, 0x10, 0x06, 0x00, 0xFF,
    "MD Apparent Export", "kVA", 0x04);

const OBISCode OBISCodes::MD_KVA_IMPORT_RATE[8] = {
    OBISCode(0x01, 0x00, 0x09, 0x06, 0x01, 0xFF, "MD kVA Import Rate 1", "kVA", 0x04),
    OBISCode(0x01, 0x00, 0x09, 0x06, 0x02, 0xFF, "MD kVA Import Rate 2", "kVA", 0x04),
    OBISCode(0x01, 0x00, 0x09, 0x06, 0x03, 0xFF, "MD kVA Import Rate 3", "kVA", 0x04),
    OBISCode(0x01, 0x00, 0x09, 0x06, 0x04, 0xFF, "MD kVA Import Rate 4", "kVA", 0x04),
    OBISCode(0x01, 0x00, 0x09, 0x06, 0x05, 0xFF, "MD kVA Import Rate 5", "kVA", 0x04),
    OBISCode(0x01, 0x00, 0x09, 0x06, 0x06, 0xFF, "MD kVA Import Rate 6", "kVA", 0x04),
    OBISCode(0x01, 0x00, 0x09, 0x06, 0x07, 0xFF, "MD kVA Import Rate 7", "kVA", 0x04),
    OBISCode(0x01, 0x00, 0x09, 0x06, 0x08, 0xFF, "MD kVA Import Rate 8", "kVA", 0x04)
};

// ============================================
// INSTANTANEOUS - VOLTAGE
// ============================================
const OBISCode OBISCodes::VOLTAGE_R(0x01, 0x00, 0x20, 0x07, 0x00, 0xFF,
    "Voltage Phase R", "V", 0x03);
const OBISCode OBISCodes::VOLTAGE_Y(0x01, 0x00, 0x34, 0x07, 0x00, 0xFF,
    "Voltage Phase Y", "V", 0x03);
const OBISCode OBISCodes::VOLTAGE_B(0x01, 0x00, 0x48, 0x07, 0x00, 0xFF,
    "Voltage Phase B", "V", 0x03);

// ============================================
// INSTANTANEOUS - CURRENT
// ============================================
const OBISCode OBISCodes::CURRENT_R(0x01, 0x00, 0x1F, 0x07, 0x00, 0xFF,
    "Current Phase R", "A", 0x03);
const OBISCode OBISCodes::CURRENT_Y(0x01, 0x00, 0x33, 0x07, 0x00, 0xFF,
    "Current Phase Y", "A", 0x03);
const OBISCode OBISCodes::CURRENT_B(0x01, 0x00, 0x47, 0x07, 0x00, 0xFF,
    "Current Phase B", "A", 0x03);
const OBISCode OBISCodes::CURRENT_NEUTRAL(0x01, 0x00, 0x5B, 0x07, 0x00, 0xFF,
    "Current Neutral", "A", 0x03);

// ============================================
// INSTANTANEOUS - POWER QUALITY
// ============================================
const OBISCode OBISCodes::POWER_FACTOR(0x01, 0x00, 0x0D, 0x07, 0x00, 0xFF,
    "Power Factor", "", 0x03);
const OBISCode OBISCodes::FREQUENCY(0x01, 0x00, 0x0E, 0x07, 0x00, 0xFF,
    "Frequency", "Hz", 0x03);

// ============================================
// CONFIGURATION
// ============================================
const OBISCode OBISCodes::MULTIPLICATION_FACTOR(0x01, 0x00, 0x00, 0x04, 0x03, 0xFF,
    "Multiplication Factor", "", 0x01);

/**
 * @brief Get OBIS code by name (simple linear search)
 */
const OBISCode* OBISCodes::getByName(const char* name) {
    // This is a simplified version - in production you might use a map
    if (strcmp(name, "Serial Number") == 0) return &METER_SERIAL_NUMBER;
    if (strcmp(name, "Manufacturer") == 0) return &METER_MANUFACTURER;
    if (strcmp(name, "kWh Import") == 0) return &KWH_IMPORT;
    // Add more as needed
    return nullptr;
}

/**
 * @brief Print all available OBIS codes
 */
void OBISCodes::printAll() {
    Serial.println("\n========== Available OBIS Codes ==========");
    Serial.println("Serial Number: " + METER_SERIAL_NUMBER.toString());
    Serial.println("Manufacturer: " + METER_MANUFACTURER.toString());
    Serial.println("kWh Import: " + KWH_IMPORT.toString());
    Serial.println("kVAh Import: " + KVAH_IMPORT.toString());
    Serial.println("MD kW Import: " + MD_KW_IMPORT.toString());
    Serial.println("MD kVA Import: " + MD_KVA_IMPORT.toString());
    Serial.println("Voltage R: " + VOLTAGE_R.toString());
    Serial.println("Current R: " + CURRENT_R.toString());
    Serial.println("Power Factor: " + POWER_FACTOR.toString());
    Serial.println("Frequency: " + FREQUENCY.toString());
    Serial.println("==========================================\n");
}
