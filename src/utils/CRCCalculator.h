/**
 * @file CRCCalculator.h
 * @brief CRC16-X25 (HDLC/DLMS) calculation utilities
 * @version 2.0
 * @date 2025-10-02
 */

#ifndef CRC_CALCULATOR_H
#define CRC_CALCULATOR_H

#include <Arduino.h>

/**
 * @class CRCCalculator
 * @brief Handles CRC16-X25 checksum calculation for HDLC/DLMS frames
 */
class CRCCalculator {
public:
    /**
     * @brief Calculate CRC16-X25 checksum
     * @param data Pointer to data buffer
     * @param length Length of data
     * @return 16-bit CRC value
     */
    static uint16_t calculate(const uint8_t* data, uint16_t length);
    
    /**
     * @brief Get LSB of last calculated CRC
     * @return Least significant byte
     */
    static uint8_t getLSB();
    
    /**
     * @brief Get MSB of last calculated CRC
     * @return Most significant byte
     */
    static uint8_t getMSB();
    
    /**
     * @brief Verify CRC of received frame
     * @param data Pointer to data buffer (including CRC bytes)
     * @param length Total length including CRC
     * @return true if CRC is valid
     */
    static bool verify(const uint8_t* data, uint16_t length);

private:
    static uint8_t crcLSB;
    static uint8_t crcMSB;
    static const uint16_t POLYNOMIAL = 0x8408; // Reversed polynomial
    static const uint16_t INITIAL_VALUE = 0xFFFF;
};

#endif // CRC_CALCULATOR_H 
