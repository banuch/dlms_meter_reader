 /**
 * @file MeterData.h
 * @brief Data structures for storing meter readings
 * @version 2.0
 * @date 2025-10-02
 */

#ifndef METER_DATA_H
#define METER_DATA_H

#include <Arduino.h>
#include <ArduinoJson.h>

/**
 * @struct MaximumDemand
 * @brief Stores MD value with occurrence timestamp
 */
struct MaximumDemand {
    float value;
    String timestamp;
    
    MaximumDemand() : value(0.0), timestamp("") {}
    
    void clear() {
        value = 0.0;
        timestamp = "";
    }
};

/**
 * @struct TODData
 * @brief Time-of-Day billing data for one zone
 */
struct TODData {
    float kwh;
    float kvah;
    float mdKW;
    float mdKVA;
    String kwhTimestamp;
    String kvahTimestamp;
    String mdKWTimestamp;
    String mdKVATimestamp;
    
    TODData() : kwh(0), kvah(0), mdKW(0), mdKVA(0) {}
    
    void clear() {
        kwh = kvah = mdKW = mdKVA = 0;
        kwhTimestamp = kvahTimestamp = mdKWTimestamp = mdKVATimestamp = "";
    }
};

/**
 * @class MeterData
 * @brief Complete meter data storage and management
 */
class MeterData {
public:
    // Meter identification
    String serialNumber;
    String manufacturer;
    String meterType;
    float multiplicationFactor;
    
    // Energy counters - Import
    float kwhImport;
    float kvahImport;
    float kvarhLag;
    float kvarhLead;
    
    // Energy counters - Export
    float kwhExport;
    float kvahExport;
    
    // Maximum Demand - Import
    MaximumDemand mdKWImport;
    MaximumDemand mdKVAImport;
    
    // Maximum Demand - Export
    MaximumDemand mdKWExport;
    MaximumDemand mdKVAExport;
    
    // Instantaneous voltage
    float voltageR;
    float voltageY;
    float voltageB;
    
    // Instantaneous current
    float currentR;
    float currentY;
    float currentB;
    float currentNeutral;
    
    // Power quality
    float powerFactor;
    float frequency;
    
    // Time-of-Day data (8 zones)
    TODData todZones[8];
    
    // Metadata
    unsigned long lastReadTime;
    String lastReadTimestamp;
    bool dataValid;
    uint8_t errorCount;
    
    /**
     * @brief Constructor
     */
    MeterData();
    
    /**
     * @brief Clear all data
     */
    void clear();
    
    /**
     * @brief Check if essential data is valid
     * @return true if data appears valid
     */
    bool isValid() const;
    
    /**
     * @brief Convert to JSON document
     * @param doc JsonDocument to populate
     * @param includeTOD Include TOD data (makes JSON larger)
     * @return true if successful
     */
    bool toJson(JsonDocument& doc, bool includeTOD = true) const;
    
    /**
     * @brief Create JSON string
     * @param includeTOD Include TOD data
     * @return JSON string
     */
    String toJsonString(bool includeTOD = true) const;
    
    /**
     * @brief Load from JSON document
     * @param doc JsonDocument to parse
     * @return true if successful
     */
    bool fromJson(const JsonDocument& doc);
    
    /**
     * @brief Print formatted data to Serial
     */
    void print() const;
    
    /**
     * @brief Print compact summary
     */
    void printSummary() const;
    
    /**
     * @brief Calculate total consumption (all TOD zones)
     * @return Total kWh from all zones
     */
    float getTotalTODkWh() const;
    
    /**
     * @brief Calculate total apparent energy (all TOD zones)
     * @return Total kVAh from all zones
     */
    float getTotalTODkVAh() const;
};

#endif // METER_DATA_H
