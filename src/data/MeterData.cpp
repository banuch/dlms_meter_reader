/**
 * @file MeterData.cpp
 * @brief Implementation of meter data management
 * @version 2.0
 * @date 2025-10-02
 */

#include "MeterData.h"

/**
 * @brief Constructor - initialize all values
 */
MeterData::MeterData() {
    clear();
}

/**
 * @brief Clear all meter data
 */
void MeterData::clear() {
    serialNumber = "";
    manufacturer = "";
    meterType = "";
    multiplicationFactor = 1.0;
    
    kwhImport = kvahImport = kvarhLag = kvarhLead = 0.0;
    kwhExport = kvahExport = 0.0;
    
    mdKWImport.clear();
    mdKVAImport.clear();
    mdKWExport.clear();
    mdKVAExport.clear();
    
    voltageR = voltageY = voltageB = 0.0;
    currentR = currentY = currentB = currentNeutral = 0.0;
    powerFactor = frequency = 0.0;
    
    for (int i = 0; i < 8; i++) {
        todZones[i].clear();
    }
    
    lastReadTime = 0;
    lastReadTimestamp = "";
    dataValid = false;
    errorCount = 0;
}

/**
 * @brief Check if data appears valid
 */
bool MeterData::isValid() const {
    return dataValid && 
           !serialNumber.isEmpty() && 
           (kwhImport > 0 || kvahImport > 0);
}

/**
 * @brief Convert meter data to JSON
 */
bool MeterData::toJson(JsonDocument& doc, bool includeTOD) const {
    doc.clear();
    
    // Meter info
    JsonObject info = doc.createNestedObject("meter");
    info["serial"] = serialNumber;
    info["manufacturer"] = manufacturer;
    info["type"] = meterType;
    info["mf"] = multiplicationFactor;
    
    // Energy - Import
    JsonObject energy = doc.createNestedObject("energy");
    energy["kwh_import"] = kwhImport;
    energy["kvah_import"] = kvahImport;
    energy["kwh_export"] = kwhExport;
    energy["kvah_export"] = kvahExport;
    energy["kvarh_lag"] = kvarhLag;
    energy["kvarh_lead"] = kvarhLead;
    
    // Maximum Demand
    JsonObject md = doc.createNestedObject("maximum_demand");
    md["kw_import"] = mdKWImport.value;
    md["kw_import_time"] = mdKWImport.timestamp;
    md["kva_import"] = mdKVAImport.value;
    md["kva_import_time"] = mdKVAImport.timestamp;
    md["kw_export"] = mdKWExport.value;
    md["kw_export_time"] = mdKWExport.timestamp;
    md["kva_export"] = mdKVAExport.value;
    md["kva_export_time"] = mdKVAExport.timestamp;
    
    // Instantaneous values
    JsonObject instant = doc.createNestedObject("instantaneous");
    JsonObject voltage = instant.createNestedObject("voltage");
    voltage["r"] = voltageR;
    voltage["y"] = voltageY;
    voltage["b"] = voltageB;
    
    JsonObject current = instant.createNestedObject("current");
    current["r"] = currentR;
    current["y"] = currentY;
    current["b"] = currentB;
    current["n"] = currentNeutral;
    
    instant["power_factor"] = powerFactor;
    instant["frequency"] = frequency;
    
    // TOD data (optional)
    if (includeTOD) {
        JsonArray tod = doc.createNestedArray("tod_zones");
        for (int i = 0; i < 8; i++) {
            JsonObject zone = tod.createNestedObject();
            zone["zone"] = i + 1;
            zone["kwh"] = todZones[i].kwh;
            zone["kvah"] = todZones[i].kvah;
            zone["md_kw"] = todZones[i].mdKW;
            zone["md_kva"] = todZones[i].mdKVA;
            if (!todZones[i].mdKWTimestamp.isEmpty()) {
                zone["md_kw_time"] = todZones[i].mdKWTimestamp;
            }
            if (!todZones[i].mdKVATimestamp.isEmpty()) {
                zone["md_kva_time"] = todZones[i].mdKVATimestamp;
            }
        }
    }
    
    // Metadata
    doc["timestamp"] = lastReadTimestamp;
    doc["valid"] = dataValid;
    doc["error_count"] = errorCount;
    
    return true;
}

/**
 * @brief Convert to JSON string
 */
String MeterData::toJsonString(bool includeTOD) const {
    DynamicJsonDocument doc(4096);
    toJson(doc, includeTOD);
    
    String output;
    serializeJson(doc, output);
    return output;
}

/**
 * @brief Load from JSON document
 */
bool MeterData::fromJson(const JsonDocument& doc) {
    try {
        if (doc.containsKey("meter")) {
            serialNumber = doc["meter"]["serial"].as<String>();
            manufacturer = doc["meter"]["manufacturer"].as<String>();
            meterType = doc["meter"]["type"].as<String>();
            multiplicationFactor = doc["meter"]["mf"] | 1.0;
        }
        
        if (doc.containsKey("energy")) {
            kwhImport = doc["energy"]["kwh_import"] | 0.0;
            kvahImport = doc["energy"]["kvah_import"] | 0.0;
            kwhExport = doc["energy"]["kwh_export"] | 0.0;
            kvahExport = doc["energy"]["kvah_export"] | 0.0;
            kvarhLag = doc["energy"]["kvarh_lag"] | 0.0;
            kvarhLead = doc["energy"]["kvarh_lead"] | 0.0;
        }
        
        // Add more parsing as needed
        
        lastReadTimestamp = doc["timestamp"].as<String>();
        dataValid = doc["valid"] | false;
        errorCount = doc["error_count"] | 0;
        
        return true;
    } catch (...) {
        return false;
    }
}

/**
 * @brief Print detailed meter data
 */
void MeterData::print() const {
    Serial.println("\n========== METER DATA ==========");
    Serial.println("Serial Number: " + serialNumber);
    Serial.println("Manufacturer: " + manufacturer);
    Serial.println("MF: " + String(multiplicationFactor));
    Serial.println("\n--- Energy ---");
    Serial.println("kWh Import: " + String(kwhImport, 3));
    Serial.println("kVAh Import: " + String(kvahImport, 3));
    Serial.println("kWh Export: " + String(kwhExport, 3));
    Serial.println("kVAh Export: " + String(kvahExport, 3));
    Serial.println("kVArh Lag: " + String(kvarhLag, 3));
    Serial.println("kVArh Lead: " + String(kvarhLead, 3));
    
    Serial.println("\n--- Maximum Demand ---");
    Serial.println("MD kW Import: " + String(mdKWImport.value, 3) + " @ " + mdKWImport.timestamp);
    Serial.println("MD kVA Import: " + String(mdKVAImport.value, 3) + " @ " + mdKVAImport.timestamp);
    
    Serial.println("\n--- Instantaneous Values ---");
    Serial.println("Voltage R/Y/B: " + String(voltageR, 1) + "/" + 
                   String(voltageY, 1) + "/" + String(voltageB, 1) + " V");
    Serial.println("Current R/Y/B: " + String(currentR, 2) + "/" + 
                   String(currentY, 2) + "/" + String(currentB, 2) + " A");
    Serial.println("Power Factor: " + String(powerFactor, 3));
    Serial.println("Frequency: " + String(frequency, 2) + " Hz");
    
    Serial.println("\n--- TOD Zones ---");
    for (int i = 0; i < 8; i++) {
        if (todZones[i].kwh > 0 || todZones[i].kvah > 0) {
            Serial.println("Zone " + String(i + 1) + ": kWh=" + String(todZones[i].kwh, 3) + 
                          ", kVAh=" + String(todZones[i].kvah, 3));
        }
    }
    
    Serial.println("\nTimestamp: " + lastReadTimestamp);
    Serial.println("Valid: " + String(dataValid ? "Yes" : "No"));
    Serial.println("================================\n");
}

/**
 * @brief Print compact summary
 */
void MeterData::printSummary() const {
    Serial.println("\n--- Meter Summary ---");
    Serial.println("S/N: " + serialNumber + " | " + manufacturer);
    Serial.println("kWh: " + String(kwhImport, 2) + " | MD: " + String(mdKWImport.value, 2) + " kW");
    Serial.println("V(R/Y/B): " + String(voltageR, 0) + "/" + String(voltageY, 0) + "/" + String(voltageB, 0));
    Serial.println("PF: " + String(powerFactor, 2) + " | Freq: " + String(frequency, 1) + " Hz");
    Serial.println("---------------------\n");
}

/**
 * @brief Calculate total TOD kWh
 */
float MeterData::getTotalTODkWh() const {
    float total = 0;
    for (int i = 0; i < 8; i++) {
        total += todZones[i].kwh;
    }
    return total;
}

/**
 * @brief Calculate total TOD kVAh
 */
float MeterData::getTotalTODkVAh() const {
    float total = 0;
    for (int i = 0; i < 8; i++) {
        total += todZones[i].kvah;
    }
    return total;
}
