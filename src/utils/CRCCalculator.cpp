/**
 * @file CRCCalculator.cpp
 * @brief Implementation of CRC16-X25 calculator
 * @version 2.0
 * @date 2025-10-02
 */

#include "CRCCalculator.h"

// Initialize static members
uint8_t CRCCalculator::crcLSB = 0;
uint8_t CRCCalculator::crcMSB = 0;

/**
 * @brief Calculate CRC16-X25 checksum for HDLC/DLMS frames
 * Algorithm: CRC-16/X-25 (polynomial 0x1021, reversed 0x8408)
 */
uint16_t CRCCalculator::calculate(const uint8_t* data, uint16_t length) {
    uint16_t crc = INITIAL_VALUE;
    
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ POLYNOMIAL;
            } else {
                crc >>= 1;
            }
        }
    }
    
    // Final XOR
    crc = ~crc;
    
    // Store MSB and LSB (note the byte order for HDLC)
    crcMSB = (crc & 0xFF);        // Low byte goes to MSB position
    crcLSB = ((crc >> 8) & 0xFF); // High byte goes to LSB position
    
    return crc;
}

/**
 * @brief Get the least significant byte of last CRC calculation
 */
uint8_t CRCCalculator::getLSB() {
    return crcLSB;
}

/**
 * @brief Get the most significant byte of last CRC calculation
 */
uint8_t CRCCalculator::getMSB() {
    return crcMSB;
}

/**
 * @brief Verify CRC of a received frame
 * @param data Complete frame including CRC bytes at the end
 * @param length Total length of frame
 * @return true if CRC is valid, false otherwise
 */
bool CRCCalculator::verify(const uint8_t* data, uint16_t length) {
    if (length < 2) return false;
    
    // Calculate CRC of data (excluding last 2 CRC bytes)
    uint16_t calculatedCRC = calculate(data, length - 2);
    
    // Extract received CRC (last 2 bytes)
    uint16_t receivedCRC = (data[length - 1] << 8) | data[length - 2];
    
    return (calculatedCRC == receivedCRC);
}