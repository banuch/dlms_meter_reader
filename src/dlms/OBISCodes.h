 /**
 * @file OBISCodes.h
 * @brief OBIS code definitions for DLMS meters
 * @version 2.0
 * @date 2025-10-02
 * 
 * OBIS Format: A-B:C.D.E*F
 * A: Medium (0=Abstract, 1=Electricity, 6=Heat, 7=Gas, 8=Water)
 * B: Channel (0=no channel)
 * C: Physical value (voltage, current, power, etc.)
 * D: Measurement type (instantaneous, max, min, average)
 * E: Tariff rate (0=total, 1-8=rate)
 * F: Billing period
 */

#ifndef OBIS_CODES_H
#define OBIS_CODES_H

#include <Arduino.h>

/**
 * @class OBISCode
 * @brief Represents a single OBIS code with metadata
 */
struct OBISCode {
    uint8_t bytes[6];
    const char* name;
    const char* unit;
    uint8_t classId;
    
    OBISCode(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f,
             const char* n, const char* u, uint8_t cls = 0x03)
        : name(n), unit(u), classId(cls) {
        bytes[0] = a;
        bytes[1] = b;
        bytes[2] = c;
        bytes[3] = d;
        bytes[4] = e;
        bytes[5] = f;
    }
    
    String toString() const {
        char buffer[20];
        sprintf(buffer, "%d-%d:%d.%d.%d*%d", 
                bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
        return String(buffer);
    }
};

/**
 * @class OBISCodes
 * @brief Collection of standard OBIS codes for energy meters
 */
class OBISCodes {
public:
    // ============================================
    // METER IDENTIFICATION
    // ============================================
    static const OBISCode METER_SERIAL_NUMBER;
    static const OBISCode METER_MANUFACTURER;
    static const OBISCode METER_TYPE;
    
    // ============================================
    // ENERGY - IMPORT (Active)
    // ============================================
    static const OBISCode KWH_IMPORT;           // Total active import
    static const OBISCode KWH_EXPORT;           // Total active export
    static const OBISCode KWH_IMPORT_RATE[8];   // Rate 1-8 import
    
    // ============================================
    // ENERGY - APPARENT
    // ============================================
    static const OBISCode KVAH_IMPORT;          // Total apparent import
    static const OBISCode KVAH_EXPORT;          // Total apparent export
    static const OBISCode KVAH_IMPORT_RATE[8];  // Rate 1-8 import
    
    // ============================================
    // ENERGY - REACTIVE
    // ============================================
    static const OBISCode KVARH_LAG;            // Reactive lag (inductive)
    static const OBISCode KVARH_LEAD;           // Reactive lead (capacitive)
    
    // ============================================
    // MAXIMUM DEMAND - ACTIVE
    // ============================================
    static const OBISCode MD_KW_IMPORT;         // MD import with timestamp
    static const OBISCode MD_KW_EXPORT;         // MD export with timestamp
    static const OBISCode MD_KW_IMPORT_RATE[8]; // MD per rate
    
    // ============================================
    // MAXIMUM DEMAND - APPARENT
    // ============================================
    static const OBISCode MD_KVA_IMPORT;        // MD apparent import
    static const OBISCode MD_KVA_EXPORT;        // MD apparent export
    static const OBISCode MD_KVA_IMPORT_RATE[8];// MD per rate
    
    // ============================================
    // INSTANTANEOUS VALUES - VOLTAGE
    // ============================================
    static const OBISCode VOLTAGE_R;            // Phase R voltage
    static const OBISCode VOLTAGE_Y;            // Phase Y voltage
    static const OBISCode VOLTAGE_B;            // Phase B voltage
    
    // ============================================
    // INSTANTANEOUS VALUES - CURRENT
    // ============================================
    static const OBISCode CURRENT_R;            // Phase R current
    static const OBISCode CURRENT_Y;            // Phase Y current
    static const OBISCode CURRENT_B;            // Phase B current
    static const OBISCode CURRENT_NEUTRAL;      // Neutral current
    
    // ============================================
    // INSTANTANEOUS VALUES - POWER
    // ============================================
    static const OBISCode POWER_FACTOR;         // Power factor
    static const OBISCode FREQUENCY;            // Line frequency
    
    // ============================================
    // CONFIGURATION
    // ============================================
    static const OBISCode MULTIPLICATION_FACTOR;// Meter MF/CT ratio
    
    /**
     * @brief Get OBIS code by name
     * @param name Name of the parameter
     * @return Pointer to OBISCode or nullptr if not found
     */
    static const OBISCode* getByName(const char* name);
    
    /**
     * @brief Print all available OBIS codes
     */
    static void printAll();
};

#endif // OBIS_CODES_H
