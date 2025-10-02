 /**
 * @file config.h
 * @brief System-wide configuration for DLMS Meter Reader
 * @version 2.0
 * @date 2025-10-02
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================
// SYSTEM CONFIGURATION
// ============================================
#define FIRMWARE_VERSION    "2.0.0"
#define DEVICE_NAME         "DLMS-Meter-Reader"
#define MANUFACTURER        "YourCompany"

// ============================================
// DLMS PROTOCOL CONFIGURATION
// ============================================
#define DLMS_PASSWORD       "1111111111111111"  // 16-byte LL password (HPL)
#define DLMS_CLIENT_SAP     0x41                // Client Service Access Point
#define DLMS_SERVER_SAP     0x03                // Server Service Access Point

// Frame configuration
#define HDLC_FLAG           0x7E
#define MAX_FRAME_SIZE      256
#define MAX_RETRY_COUNT     3

// ============================================
// CLOUD CONFIGURATION
// ============================================
// WiFi Settings
#define WIFI_SSID           "Sahasra"
#define WIFI_PASSWORD       "wintek@143"
#define WIFI_TIMEOUT        30000  // 30 seconds
#define WIFI_RETRY_DELAY    5000   // 5 seconds between retries

// MQTT Settings
#define MQTT_ENABLED        true
#define MQTT_BROKER         "broker.hivemq.com"  // Change to your broker
#define MQTT_PORT           1883
#define MQTT_USER           ""                    // If required
#define MQTT_PASSWORD       ""                    // If required
#define MQTT_CLIENT_ID      "DLMS_Meter_"        // Will append MAC address
#define MQTT_KEEPALIVE      60

// MQTT Topics
#define MQTT_TOPIC_BASE     "dlms/meter/"        // Will append meter ID
#define MQTT_TOPIC_DATA     "data"
#define MQTT_TOPIC_STATUS   "status"
#define MQTT_TOPIC_ERROR    "error"
#define MQTT_TOPIC_CMD      "command"

// HTTP/REST API Settings
#define HTTP_ENABLED        false
#define API_ENDPOINT        "http://your-api.com/meter/data"
#define API_KEY             "your-api-key-here"

// ThingSpeak Settings (Optional)
#define THINGSPEAK_ENABLED  false
#define THINGSPEAK_API_KEY  "YOUR_WRITE_API_KEY"
#define THINGSPEAK_CHANNEL  123456

// ============================================
// DATA COLLECTION CONFIGURATION
// ============================================
#define READ_INTERVAL       60000   // ms between readings (60 seconds)
#define UPLOAD_INTERVAL     300000  // ms between cloud uploads (5 minutes)
#define MAX_OFFLINE_BUFFER  100     // Maximum readings to store offline

// Time-of-Day (TOD) Configuration
#define TOD_ZONES           8       // Number of TOD billing zones

// ============================================
// LOGGING CONFIGURATION
// ============================================
#define DEBUG_MODE          true
#define LOG_LEVEL_ERROR     0
#define LOG_LEVEL_WARN      1
#define LOG_LEVEL_INFO      2
#define LOG_LEVEL_DEBUG     3
#define LOG_LEVEL           LOG_LEVEL_DEBUG

// ============================================
// NTP TIME SYNC
// ============================================
#define NTP_ENABLED         true
#define NTP_SERVER          "pool.ntp.org"
#define NTP_TIMEZONE        19800   // IST (UTC+5:30) in seconds
#define NTP_UPDATE_INTERVAL 3600000 // 1 hour

// ============================================
// STORAGE CONFIGURATION
// ============================================
#define SPIFFS_ENABLED      true
#define PREFERENCES_ENABLED true
#define CONFIG_FILE         "/config.json"
#define DATA_FILE           "/meter_data.json"

// ============================================
// FEATURE FLAGS
// ============================================
#define ENABLE_OTA_UPDATE   true
#define ENABLE_WEB_SERVER   true
#define ENABLE_DISPLAY      false
#define ENABLE_AUTO_RESTART true
#define AUTO_RESTART_HOURS  24      // Restart every 24 hours

// ============================================
// ERROR HANDLING
// ============================================
#define MAX_CONSECUTIVE_ERRORS  5
#define ERROR_RECOVERY_DELAY    10000  // 10 seconds
#define WATCHDOG_TIMEOUT        30000  // 30 seconds

#endif // CONFIG_H
