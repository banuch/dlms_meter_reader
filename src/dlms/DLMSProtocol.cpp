 /**
 * @file DLMSProtocol.cpp
 * @brief Complete DLMS/COSEM protocol implementation
 * @version 2.0
 * @date 2025-10-02
 */

#include "DLMSProtocol.h"

// ============================================
// STATIC FRAME DEFINITIONS
// ============================================

const uint8_t DLMSProtocol::SNRM_FRAME[] = {
    0x7E, 0xA0, 0x20, 0x03, 0x41, 0x93, 0x28, 0xBC,
    0x81, 0x80, 0x14, 0x05, 0x02, 0x05, 0x01, 0x06,
    0x02, 0x05, 0x01, 0x07, 0x04, 0x00, 0x00, 0x00,
    0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01, 0xDD,
    0x70, 0x7E
};

const uint8_t DLMSProtocol::AARQ_FRAME[] = {
    0x7E, 0xA0, 0x4C, 0x03, 0x41, 0x10, 0x6B, 0x04,
    0xE6, 0xE6, 0x00, 0x60, 0x3E, 0xA1, 0x09, 0x06,
    0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,
    0x8A, 0x02, 0x07, 0x80, 0x8B, 0x07, 0x60, 0x85,
    0x74, 0x05, 0x08, 0x02, 0x01, 0xAC, 0x12, 0x80,
    0x10, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
    0x31, 0xBE, 0x10, 0x04, 0x0E, 0x01, 0x00, 0x00,
    0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x18,
    0x1D, 0xFF, 0xFF, 0xB3, 0x3D, 0x7E
};

const uint8_t DLMSProtocol::DISC_FRAME[] = {
    0x7E, 0xA0, 0x07, 0x03, 0x41, 0x53, 0x56, 0xA2, 0x7E
};

// ============================================
// CONSTRUCTOR & INITIALIZATION
// ============================================

DLMSProtocol::DLMSProtocol() 
    : state(DLMSState::DISCONNECTED),
      lastError(DLMSError::NONE),
      errorCount(0),
      hdlcFrameCounter(0x10),
      receiveLength(0) {
    memset(receiveBuffer, 0, sizeof(receiveBuffer));
}

void DLMSProtocol::begin() {
    LOG_INFO("DLMS Protocol initialized");
    state = DLMSState::DISCONNECTED;
    hdlcFrameCounter = 0x10;
    resetErrors();
}

// ============================================
// CONNECTION MANAGEMENT
// ============================================

bool DLMSProtocol::connect() {
    LOG_INFO("=== Starting DLMS Connection ===");
    
    // Wake up meter
    HardwareManager::wakeupMeter();
    HardwareManager::clearRxBuffer();
    
    // Step 1: SNRM
    if (!sendSNRM()) {
        setError(DLMSError::TIMEOUT);
        LOG_ERROR("SNRM failed");
        return false;
    }
    
    delay(100);
    
    // Step 2: AARQ
    if (!sendAARQ()) {
        setError(DLMSError::AUTHENTICATION_FAILED);
        LOG_ERROR("AARQ failed");
        return false;
    }
    
    state = DLMSState::ASSOCIATED;
    LOG_INFO("=== DLMS Connected Successfully ===");
    HardwareManager::showSuccess();
    
    return true;
}

bool DLMSProtocol::disconnect() {
    LOG_INFO("Disconnecting from meter...");
    
    bool success = sendDisconnect();
    delay(100);
    sendDisconnect(); // Send twice for reliability
    
    state = DLMSState::DISCONNECTED;
    hdlcFrameCounter = 0x10;
    
    HardwareManager::sleepMeter();
    LOG_INFO("Disconnected");
    
    return success;
}

// ============================================
// FRAME SENDING & RECEIVING
// ============================================

bool DLMSProtocol::sendSNRM() {
    LOG_INFO(">>> Sending SNRM");
    
    if (!sendFrame(SNRM_FRAME, sizeof(SNRM_FRAME))) {
        return false;
    }
    
    if (!receiveFrame()) {
        return false;
    }
    
    return verifySNRMResponse();
}

bool DLMSProtocol::sendAARQ() {
    LOG_INFO(">>> Sending AARQ");
    
    if (!sendFrame(AARQ_FRAME, sizeof(AARQ_FRAME))) {
        return false;
    }
    
    if (!receiveFrame()) {
        return false;
    }
    
    return verifyAAREResponse();
}

bool DLMSProtocol::sendDisconnect() {
    LOG_DEBUG(">>> Sending DISCONNECT");
    
    if (!sendFrame(DISC_FRAME, sizeof(DISC_FRAME))) {
        return false;
    }
    
    receiveFrame(500); // Short timeout for DISC response
    return true;
}

bool DLMSProtocol::sendFrame(const uint8_t* frame, uint16_t length) {
    HardwareManager::showActivity();
    HardwareManager::write(frame, length);
    HardwareManager::flush();
    
    LOG_HEX("TX", frame, length);
    
    return true;
}

bool DLMSProtocol::receiveFrame(uint32_t timeout) {
    memset(receiveBuffer, 0, sizeof(receiveBuffer));
    receiveLength = 0;
    
    uint32_t startTime = millis();
    bool frameStarted = false;
    
    while (millis() - startTime < timeout) {
        if (HardwareManager::available()) {
            uint8_t byte = HardwareManager::read();
            
            // Look for frame start
            if (byte == HDLC_FLAG) {
                frameStarted = true;
                receiveLength = 0;
            }
            
            if (frameStarted && receiveLength < MAX_FRAME_SIZE) {
                receiveBuffer[receiveLength++] = byte;
                
                // Check for frame end
                if (receiveLength > 2 && byte == HDLC_FLAG && receiveBuffer[0] == HDLC_FLAG) {
                    LOG_HEX("RX", receiveBuffer, receiveLength);
                    return true;
                }
            }
        }
        delay(1);
    }
    
    LOG_ERROR("Receive timeout");
    return false;
}

// ============================================
// RESPONSE VERIFICATION
// ============================================

bool DLMSProtocol::verifySNRMResponse() {
    // UA frame format: 7E A0 xx 41 03 73 ... 7E
    if (receiveLength < 7) {
        LOG_ERROR("SNRM response too short");
        return false;
    }
    
    if (receiveBuffer[0] != 0x7E || 
        receiveBuffer[1] != 0xA0 ||
        receiveBuffer[3] != 0x41 ||
        receiveBuffer[4] != 0x03 ||
        receiveBuffer[5] != 0x73) {
        LOG_ERROR("Invalid SNRM response");
        return false;
    }
    
    LOG_INFO("SNRM Response OK");
    state = DLMSState::CONNECTED;
    return true;
}

bool DLMSProtocol::verifyAAREResponse() {
    // AARE frame format: 7E A0 xx 41 03 ... E6 E7 00 ... 00 ... 7E
    if (receiveLength < 30) {
        LOG_ERROR("AARE response too short");
        return false;
    }
    
    if (receiveBuffer[0] != 0x7E ||
        receiveBuffer[1] != 0xA0 ||
        receiveBuffer[3] != 0x41 ||
        receiveBuffer[4] != 0x03 ||
        receiveBuffer[8] != 0xE6 ||
        receiveBuffer[9] != 0xE7) {
        LOG_ERROR("Invalid AARE response");
        return false;
    }
    
    // Check association result (should be 0x00 = accepted)
    if (receiveBuffer[28] != 0x00) {
        LOG_ERROR("Association rejected");
        return false;
    }
    
    LOG_INFO("AARE Response OK - Association established");
    return true;
}

bool DLMSProtocol::verifyOBISResponse() {
    if (receiveLength < 15) {
        LOG_ERROR("OBIS response too short");
        return false;
    }
    
    if (receiveBuffer[0] != 0x7E ||
        receiveBuffer[1] != 0xA0 ||
        receiveBuffer[3] != 0x41 ||
        receiveBuffer[4] != 0x03 ||
        receiveBuffer[8] != 0xE6 ||
        receiveBuffer[9] != 0xE7 ||
        receiveBuffer[13] != 0xC1 ||
        receiveBuffer[14] != 0x00) {
        LOG_ERROR("Invalid OBIS response format");
        return false;
    }
    
    return true;
}

// ============================================
// OBIS READING
// ============================================

bool DLMSProtocol::readMeterData(MeterData& data) {
    LOG_INFO("=== Reading Complete Meter Data ===");
    
    data.clear();
    bool success = true;
    
    // Read identification
    if (!readOBISString(OBISCodes::METER_SERIAL_NUMBER, data.serialNumber)) {
        LOG_WARN("Failed to read serial number");
        success = false;
    }
    
    if (!readOBISString(OBISCodes::METER_MANUFACTURER, data.manufacturer)) {
        LOG_WARN("Failed to read manufacturer");
        success = false;
    }
    
    // Read energy values
    String dummy;
    readOBIS(OBISCodes::KWH_IMPORT, data.kwhImport, dummy);
    readOBIS(OBISCodes::KVAH_IMPORT, data.kvahImport, dummy);
    readOBIS(OBISCodes::KWH_EXPORT, data.kwhExport, dummy);
    readOBIS(OBISCodes::KVAH_EXPORT, data.kvahExport, dummy);
    readOBIS(OBISCodes::KVARH_LAG, data.kvarhLag, dummy);
    readOBIS(OBISCodes::KVARH_LEAD, data.kvarhLead, dummy);
    
    // Read maximum demand
    readOBIS(OBISCodes::MD_KW_IMPORT, data.mdKWImport.value, data.mdKWImport.timestamp);
    readOBIS(OBISCodes::MD_KVA_IMPORT, data.mdKVAImport.value, data.mdKVAImport.timestamp);
    
    // Read instantaneous values
    readOBIS(OBISCodes::VOLTAGE_R, data.voltageR, dummy);
    readOBIS(OBISCodes::VOLTAGE_Y, data.voltageY, dummy);
    readOBIS(OBISCodes::VOLTAGE_B, data.voltageB, dummy);
    readOBIS(OBISCodes::CURRENT_R, data.currentR, dummy);
    readOBIS(OBISCodes::CURRENT_Y, data.currentY, dummy);
    readOBIS(OBISCodes::CURRENT_B, data.currentB, dummy);
    readOBIS(OBISCodes::POWER_FACTOR, data.powerFactor, dummy);
    readOBIS(OBISCodes::FREQUENCY, data.frequency, dummy);
    
    // Read TOD zones (first 4 zones)
    for (int i = 0; i < 4; i++) {
        readOBIS(OBISCodes::KWH_IMPORT_RATE[i], data.todZones[i].kwh, data.todZones[i].kwhTimestamp);
        readOBIS(OBISCodes::KVAH_IMPORT_RATE[i], data.todZones[i].kvah, data.todZones[i].kvahTimestamp);
    }
    
    data.dataValid = true;
    data.lastReadTime = millis();
    
    // Format timestamp
    char buffer[20];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", 
            2025, 10, 2, 12, 0, 0); // You can add RTC support here
    data.lastReadTimestamp = String(buffer);
    
    LOG_INFO("=== Meter Data Read Complete ===");
    return success;
}

bool DLMSProtocol::readOBIS(const OBISCode& obis, float& value, String& timestamp) {
    value = 0.0;
    timestamp = "";
    
    LOG_DEBUG("Reading: " + String(obis.name));
    
    // Read Attribute 1 (class check)
    uint8_t frame[27];
    uint16_t len = buildOBISFrame(obis, obis.classId, 0x01, frame);
    
    if (!sendFrame(frame, len)) return false;
    if (!receiveFrame()) return false;
    if (!verifyOBISResponse()) return false;
    
    incrementFrameCounter();
    delay(50);
    
    // Read Attribute 2 (value)
    len = buildOBISFrame(obis, obis.classId, 0x02, frame);
    
    if (!sendFrame(frame, len)) return false;
    if (!receiveFrame()) return false;
    if (!verifyOBISResponse()) return false;
    
    if (!extractValue(value)) {
        LOG_WARN("Failed to extract value");
        return false;
    }
    
    incrementFrameCounter();
    delay(50);
    
    // Read Attribute 3 (scaler) for class 3
    if (obis.classId == 0x03) {
        len = buildOBISFrame(obis, obis.classId, 0x03, frame);
        
        if (sendFrame(frame, len) && receiveFrame() && verifyOBISResponse()) {
            // Apply scaler (simplified - assumes scaler in byte 18)
            if (receiveLength > 18 && receiveBuffer[18] > 127) {
                int8_t scaler = ~receiveBuffer[18];
                value = value / pow(10, scaler + 1);
            } else if (receiveLength > 18) {
                value = value * pow(10, receiveBuffer[18]);
            }
        }
        
        incrementFrameCounter();
        delay(50);
    }
    
    // Read timestamp for MD values (class 4, attribute 5)
    if (obis.classId == 0x04) {
        // Skip attribute 4, go to attribute 5
        len = buildOBISFrame(obis, obis.classId, 0x05, frame);
        
        if (sendFrame(frame, len) && receiveFrame() && verifyOBISResponse()) {
            extractDateTime(timestamp);
        }
        
        incrementFrameCounter();
    }
    
    LOG_DEBUG(String(obis.name) + ": " + String(value, 3) + " " + String(obis.unit));
    return true;
}

bool DLMSProtocol::readOBISString(const OBISCode& obis, String& value) {
    value = "";
    
    LOG_DEBUG("Reading string: " + String(obis.name));
    
    // Read Attribute 2
    uint8_t frame[27];
    uint16_t len = buildOBISFrame(obis, obis.classId, 0x02, frame);
    
    if (!sendFrame(frame, len)) return false;
    if (!receiveFrame()) return false;
    if (!verifyOBISResponse()) return false;
    
    if (!extractString(value)) {
        LOG_WARN("Failed to extract string");
        return false;
    }
    
    incrementFrameCounter();
    delay(50);
    
    LOG_DEBUG(String(obis.name) + ": " + value);
    return true;
}

// ============================================
// FRAME BUILDING
// ============================================

uint16_t DLMSProtocol::buildOBISFrame(const OBISCode& obis, uint8_t classId, 
                                       uint8_t attribute, uint8_t* frame) {
    frame[0] = 0x7E;
    frame[1] = 0xA0;
    frame[2] = 0x19;  // Length
    frame[3] = 0x03;
    frame[4] = 0x41;
    frame[5] = hdlcFrameCounter;
    frame[6] = 0x00;  // HCS MSB (calculated later)
    frame[7] = 0x00;  // HCS LSB (calculated later)
    frame[8] = 0xE6;
    frame[9] = 0xE6;
    frame[10] = 0x00;
    frame[11] = 0xC0;
    frame[12] = 0x01;
    frame[13] = 0xC1;
    frame[14] = 0x00;
    frame[15] = classId;
    frame[16] = obis.bytes[0];
    frame[17] = obis.bytes[1];
    frame[18] = obis.bytes[2];
    frame[19] = obis.bytes[3];
    frame[20] = obis.bytes[4];
    frame[21] = obis.bytes[5];
    frame[22] = attribute;
    frame[23] = 0x00;  // FCS MSB (calculated later)
    frame[24] = 0x00;  // FCS LSB (calculated later)
    frame[25] = 0x00;
    frame[26] = 0x7E;
    
    // Calculate HCS (Header Check Sequence)
    uint8_t hcsData[5] = {frame[1], frame[2], frame[3], frame[4], frame[5]};
    CRCCalculator::calculate(hcsData, 5);
    frame[6] = CRCCalculator::getMSB();
    frame[7] = CRCCalculator::getLSB();
    
    // Calculate FCS (Frame Check Sequence)
    uint8_t fcsData[23];
    for (int i = 0; i < 23; i++) {
        fcsData[i] = frame[i + 1];
    }
    CRCCalculator::calculate(fcsData, 23);
    frame[24] = CRCCalculator::getMSB();
    frame[25] = CRCCalculator::getLSB();
    
    return 27;
}

// ============================================
// DATA EXTRACTION
// ============================================

bool DLMSProtocol::extractValue(float& value) {
    if (receiveLength < 20) return false;
    
    // Find data type tag
    uint8_t dataType = receiveBuffer[15];
    
    if (dataType == 0x06) {  // 32-bit unsigned
        uint32_t temp = ((uint32_t)receiveBuffer[16] << 24) |
                       ((uint32_t)receiveBuffer[17] << 16) |
                       ((uint32_t)receiveBuffer[18] << 8) |
                       ((uint32_t)receiveBuffer[19]);
        value = (float)temp;
        return true;
    }
    else if (dataType == 0x12 || dataType == 0x10) {  // 16-bit unsigned
        uint16_t temp = ((uint16_t)receiveBuffer[16] << 8) |
                       ((uint16_t)receiveBuffer[17]);
        value = (float)temp;
        return true;
    }
    
    return false;
}

bool DLMSProtocol::extractString(String& value) {
    if (receiveLength < 18) return false;
    
    uint8_t dataType = receiveBuffer[15];
    
    if (dataType == 0x09 || dataType == 0x0A) {  // Octet string / visible string
        uint8_t length = receiveBuffer[16];
        value = "";
        for (int i = 0; i < length && (17 + i) < receiveLength; i++) {
            value += (char)receiveBuffer[17 + i];
        }
        return true;
    }
    
    return false;
}

bool DLMSProtocol::extractDateTime(String& timestamp) {
    if (receiveLength < 30) return false;
    
    // DLMS datetime: 12 bytes starting at offset 17
    uint16_t year = (receiveBuffer[17] << 8) | receiveBuffer[18];
    uint8_t month = receiveBuffer[19];
    uint8_t day = receiveBuffer[20];
    uint8_t hour = receiveBuffer[21];
    uint8_t minute = receiveBuffer[22];
    uint8_t second = receiveBuffer[23];
    
    // Handle "not specified" values
    if (year == 0xFFFF) year = 0;
    if (month == 0xFF) month = 1;
    if (day == 0xFF) day = 1;
    if (hour == 0xFF) hour = 0;
    if (minute == 0xFF) minute = 0;
    if (second == 0xFF) second = 0;
    
    char buffer[20];
    sprintf(buffer, "%04u-%02u-%02u %02u:%02u:%02u", 
            year, month, day, hour, minute, second);
    timestamp = String(buffer);
    
    return true;
}

// ============================================
// UTILITY FUNCTIONS
// ============================================

void DLMSProtocol::incrementFrameCounter() {
    if (hdlcFrameCounter < 0xFE) {
        hdlcFrameCounter += 0x22;
    } else {
        hdlcFrameCounter = 0x10;
    }
}

void DLMSProtocol::setError(DLMSError error) {
    lastError = error;
    errorCount++;
    state = DLMSState::ERROR;
}
