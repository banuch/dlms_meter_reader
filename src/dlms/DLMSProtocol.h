 /**
 * @file DLMSProtocol.h
 * @brief DLMS/COSEM protocol implementation
 * @version 2.0
 * @date 2025-10-02
 */

#ifndef DLMS_PROTOCOL_H
#define DLMS_PROTOCOL_H

#include <Arduino.h>
#include "../config/config.h"
#include "../config/pins.h"
#include "../utils/CRCCalculator.h"
#include "../utils/Logger.h"
#include "../hardware/HardwareManager.h"
#include "../data/MeterData.h"
#include "OBISCodes.h"

/**
 * @enum DLMSState
 * @brief Current state of DLMS communication
 */
enum class DLMSState {
    DISCONNECTED,
    SNRM_SENT,
    CONNECTED,
    AARQ_SENT,
    ASSOCIATED,
    READING,
    ERROR
};

/**
 * @enum DLMSError
 * @brief DLMS error codes
 */
enum class DLMSError {
    NONE = 0,
    TIMEOUT,
    INVALID_RESPONSE,
    CRC_ERROR,
    AUTHENTICATION_FAILED,
    OBIS_READ_FAILED,
    FRAME_ERROR,
    UNKNOWN
};

/**
 * @class DLMSProtocol
 * @brief Handles DLMS/COSEM protocol communication
 */
class DLMSProtocol {
public:
    /**
     * @brief Constructor
     */
    DLMSProtocol();
    
    /**
     * @brief Initialize DLMS protocol
     */
    void begin();
    
    /**
     * @brief Connect to meter (SNRM + AARQ)
     * @return true if successful
     */
    bool connect();
    
    /**
     * @brief Disconnect from meter
     * @return true if successful
     */
    bool disconnect();
    
    /**
     * @brief Read all meter data
     * @param data MeterData structure to populate
     * @return true if successful
     */
    bool readMeterData(MeterData& data);
    
    /**
     * @brief Read specific OBIS code
     * @param obis OBIS code to read
     * @param value Output value
     * @param timestamp Output timestamp (for MD values)
     * @return true if successful
     */
    bool readOBIS(const OBISCode& obis, float& value, String& timestamp);
    
    /**
     * @brief Read string data (serial number, manufacturer)
     * @param obis OBIS code
     * @param value Output string
     * @return true if successful
     */
    bool readOBISString(const OBISCode& obis, String& value);
    
    /**
     * @brief Get current state
     */
    DLMSState getState() const { return state; }
    
    /**
     * @brief Get last error
     */
    DLMSError getLastError() const { return lastError; }
    
    /**
     * @brief Get error count
     */
    uint8_t getErrorCount() const { return errorCount; }
    
    /**
     * @brief Reset error count
     */
    void resetErrors() { errorCount = 0; lastError = DLMSError::NONE; }
    
    /**
     * @brief Check if connected
     */
    bool isConnected() const { return state == DLMSState::ASSOCIATED; }

private:
    DLMSState state;
    DLMSError lastError;
    uint8_t errorCount;
    uint8_t hdlcFrameCounter;
    uint8_t receiveBuffer[MAX_FRAME_SIZE];
    uint16_t receiveLength;
    
    // Protocol frames
    static const uint8_t SNRM_FRAME[];
    static const uint8_t AARQ_FRAME[];
    static const uint8_t DISC_FRAME[];
    
    /**
     * @brief Send SNRM (Set Normal Response Mode)
     * @return true if successful
     */
    bool sendSNRM();
    
    /**
     * @brief Send AARQ (Association Request)
     * @return true if successful
     */
    bool sendAARQ();
    
    /**
     * @brief Send disconnect frame
     * @return true if successful
     */
    bool sendDisconnect();
    
    /**
     * @brief Build OBIS read request frame
     * @param obis OBIS code
     * @param classId Class ID
     * @param attribute Attribute number
     * @param frame Output frame buffer
     * @return Frame length
     */
    uint16_t buildOBISFrame(const OBISCode& obis, uint8_t classId, 
                            uint8_t attribute, uint8_t* frame);
    
    /**
     * @brief Send frame to meter
     * @param frame Frame data
     * @param length Frame length
     * @return true if successful
     */
    bool sendFrame(const uint8_t* frame, uint16_t length);
    
    /**
     * @brief Receive frame from meter
     * @param timeout Timeout in milliseconds
     * @return true if frame received
     */
    bool receiveFrame(uint32_t timeout = COMMAND_TIMEOUT);
    
    /**
     * @brief Verify SNRM response (UA frame)
     * @return true if valid
     */
    bool verifySNRMResponse();
    
    /**
     * @brief Verify AARQ response (AARE frame)
     * @return true if valid
     */
    bool verifyAAREResponse();
    
    /**
     * @brief Verify OBIS response
     * @return true if valid
     */
    bool verifyOBISResponse();
    
    /**
     * @brief Extract value from OBIS response
     * @param value Output value
     * @return true if successful
     */
    bool extractValue(float& value);
    
    /**
     * @brief Extract string from OBIS response
     * @param value Output string
     * @return true if successful
     */
    bool extractString(String& value);
    
    /**
     * @brief Extract datetime from OBIS response
     * @param timestamp Output timestamp
     * @return true if successful
     */
    bool extractDateTime(String& timestamp);
    
    /**
     * @brief Increment HDLC frame counter
     */
    void incrementFrameCounter();
    
    /**
     * @brief Set error state
     * @param error Error code
     */
    void setError(DLMSError error);
};

#endif // DLMS_PROTOCOL_H
