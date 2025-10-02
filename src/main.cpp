 /**
 * @file main.cpp
 * @brief Main application - DLMS Meter Reader with Cloud Integration
 * @version 2.0
 * @date 2025-10-02
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "config/config.h"
#include "config/pins.h"
#include "utils/Logger.h"
#include "utils/CRCCalculator.h"
#include "hardware/HardwareManager.h"
#include "dlms/DLMSProtocol.h"
#include "dlms/OBISCodes.h"
#include "data/MeterData.h"

// ============================================
// GLOBAL OBJECTS
// ============================================

DLMSProtocol dlms;
MeterData meterData;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ============================================
// TIMING VARIABLES
// ============================================

unsigned long lastReadTime = 0;
unsigned long lastUploadTime = 0;
unsigned long lastReconnectAttempt = 0;
unsigned long lastHeartbeat = 0;

// ============================================
// STATE VARIABLES
// ============================================

bool wifiConnected = false;
bool mqttConnected = false;
uint8_t consecutiveErrors = 0;
uint16_t readingCount = 0;

// ============================================
// FUNCTION DECLARATIONS
// ============================================

bool connectWiFi();
bool connectMQTT();
void reconnectMQTT();
bool publishMQTT(const String& topic, const String& payload);
bool publishHTTP(const String& jsonData);
bool readMeter();
void uploadData();
void handleErrors();
void printSystemStatus();
void mqttCallback(char* topic, byte* payload, unsigned int length);

// ============================================
// SETUP
// ============================================

void setup() {
    // Initialize serial for debugging
    Serial.begin(DEBUG_BAUD_RATE);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║   DLMS Meter Reader v" FIRMWARE_VERSION "         ║");
    Serial.println("║   Professional IoT Energy Monitor     ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();
    
    // Initialize logger
    Logger::begin(Logger::DEBUG);
    Logger::enableColors(true);
    Logger::enableTimestamp(true);
    
    LOG_INFO("System starting...");
    
    // Initialize hardware
    LOG_INFO("Initializing hardware...");
    HardwareManager::begin();
    
    // Initialize DLMS protocol
    LOG_INFO("Initializing DLMS protocol...");
    dlms.begin();
    
    // Print OBIS codes (optional)
    // OBISCodes::printAll();
    
    // Connect to WiFi
    LOG_INFO("Connecting to WiFi...");
    if (connectWiFi()) {
        wifiConnected = true;
        LOG_INFO("WiFi connected!");
        LOG_INFO("IP Address: " + WiFi.localIP().toString());
        LOG_INFO("Signal: " + String(WiFi.RSSI()) + " dBm");
        
        // Setup MQTT
        mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
        mqttClient.setCallback(mqttCallback);
        mqttClient.setKeepAlive(MQTT_KEEPALIVE);
        
        // Connect to MQTT
        if (connectMQTT()) {
            mqttConnected = true;
            LOG_INFO("MQTT connected!");
        }
    } else {
        LOG_WARN("WiFi connection failed - continuing in offline mode");
        wifiConnected = false;
    }
    
    LOG_INFO("═══════════════════════════════════════");
    LOG_INFO("    System Ready - Starting Loop");
    LOG_INFO("═══════════════════════════════════════\n");
    
    HardwareManager::showSuccess();
    delay(1000);
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {
    unsigned long currentMillis = millis();
    
    // Handle MQTT connection
    if (wifiConnected && mqttConnected) {
        if (!mqttClient.loop()) {
            mqttConnected = false;
        }
    }
    
    // Auto-reconnect MQTT
    if (wifiConnected && !mqttConnected) {
        if (currentMillis - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = currentMillis;
            reconnectMQTT();
        }
    }
    
    // Read meter data periodically
    if (currentMillis - lastReadTime >= READ_INTERVAL) {
        lastReadTime = currentMillis;
        
        LOG_INFO("\n┌─────────────────────────────────────┐");
        LOG_INFO("│  Starting Meter Reading #" + String(++readingCount) + "       │");
        LOG_INFO("└─────────────────────────────────────┘");
        
        if (readMeter()) {
            consecutiveErrors = 0;
            HardwareManager::setLED(LEDColor::GREEN);
            delay(500);
            HardwareManager::ledsOff();
        } else {
            handleErrors();
        }
    }
    
    // Upload data to cloud periodically
    if (currentMillis - lastUploadTime >= UPLOAD_INTERVAL) {
        lastUploadTime = currentMillis;
        
        if (meterData.isValid()) {
            uploadData();
        } else {
            LOG_WARN("No valid data to upload");
        }
    }
    
    // Heartbeat / status LED
    if (currentMillis - lastHeartbeat >= 2000) {
        lastHeartbeat = currentMillis;
        HardwareManager::statusLedToggle();
        
        // Optional: Print system status every minute
        static uint8_t heartbeatCount = 0;
        if (++heartbeatCount >= 30) {
            heartbeatCount = 0;
            printSystemStatus();
        }
    }
    
    // Check for WiFi reconnection
    if (!wifiConnected && currentMillis - lastReconnectAttempt > 30000) {
        lastReconnectAttempt = currentMillis;
        LOG_INFO("Attempting WiFi reconnection...");
        if (connectWiFi()) {
            wifiConnected = true;
            connectMQTT();
        }
    }
    
    delay(100);
}

// ============================================
// WIFI FUNCTIONS
// ============================================

bool connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    uint32_t startTime = millis();
    int dots = 0;
    
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > WIFI_TIMEOUT) {
            LOG_ERROR("WiFi connection timeout");
            return false;
        }
        
        delay(500);
        Serial.print(".");
        if (++dots % 50 == 0) Serial.println();
    }
    
    Serial.println();
    return true;
}

// ============================================
// MQTT FUNCTIONS
// ============================================

bool connectMQTT() {
    String clientId = String(MQTT_CLIENT_ID) + String(WiFi.macAddress());
    clientId.replace(":", "");
    
    LOG_INFO("Connecting to MQTT broker: " + String(MQTT_BROKER));
    
    bool connected = false;
    if (strlen(MQTT_USER) > 0) {
        connected = mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD);
    } else {
        connected = mqttClient.connect(clientId.c_str());
    }
    
    if (connected) {
        LOG_INFO("MQTT Client ID: " + clientId);
        
        // Subscribe to command topic
        String cmdTopic = String(MQTT_TOPIC_BASE) + meterData.serialNumber + "/" + MQTT_TOPIC_CMD;
        mqttClient.subscribe(cmdTopic.c_str());
        LOG_INFO("Subscribed to: " + cmdTopic);
        
        // Publish status
        String statusTopic = String(MQTT_TOPIC_BASE) + meterData.serialNumber + "/" + MQTT_TOPIC_STATUS;
        publishMQTT(statusTopic, "online");
        
        return true;
    }
    
    LOG_ERROR("MQTT connection failed, rc=" + String(mqttClient.state()));
    return false;
}

void reconnectMQTT() {
    LOG_INFO("Attempting MQTT reconnection...");
    if (connectMQTT()) {
        mqttConnected = true;
        LOG_INFO("MQTT reconnected");
    }
}

bool publishMQTT(const String& topic, const String& payload) {
    if (!mqttConnected) {
        LOG_WARN("MQTT not connected, cannot publish");
        return false;
    }
    
    bool result = mqttClient.publish(topic.c_str(), payload.c_str(), false);
    
    if (result) {
        LOG_DEBUG("Published to " + topic);
    } else {
        LOG_ERROR("Failed to publish to " + topic);
    }
    
    return result;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    LOG_INFO("MQTT Message [" + String(topic) + "]: " + message);
    
    // Handle commands
    if (message == "READ") {
        LOG_INFO("Remote read command received");
        readMeter();
        uploadData();
    }
    else if (message == "STATUS") {
        printSystemStatus();
        uploadData();
    }
    else if (message == "RESTART") {
        LOG_WARN("Restart command received");
        delay(1000);
        ESP.restart();
    }
}

// ============================================
// HTTP FUNCTIONS
// ============================================

bool publishHTTP(const String& jsonData) {
    if (!HTTP_ENABLED) return false;
    
    HTTPClient http;
    
    LOG_INFO("Posting to HTTP API...");
    
    http.begin(API_ENDPOINT);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-Key", API_KEY);
    http.setTimeout(5000);
    
    int httpCode = http.POST(jsonData);
    
    if (httpCode > 0) {
        LOG_INFO("HTTP Response code: " + String(httpCode));
        
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
            String response = http.getString();
            LOG_DEBUG("Response: " + response);
            http.end();
            return true;
        }
    } else {
        LOG_ERROR("HTTP POST failed: " + http.errorToString(httpCode));
    }
    
    http.end();
    return false;
}

// ============================================
// METER READING FUNCTIONS
// ============================================

bool readMeter() {
    HardwareManager::setLED(LEDColor::BLUE);
    
    LOG_INFO("Connecting to meter...");
    
    if (!dlms.connect()) {
        LOG_ERROR("Failed to connect to meter");
        HardwareManager::showError(2);
        return false;
    }
    
    LOG_INFO("Reading meter data...");
    
    bool success = dlms.readMeterData(meterData);
    
    if (success) {
        LOG_INFO("✓ Meter data read successfully");
        meterData.printSummary();
        
        // Optional: Print full data
        // meterData.print();
    } else {
        LOG_ERROR("Failed to read meter data");
        HardwareManager::showError(3);
    }
    
    dlms.disconnect();
    HardwareManager::ledsOff();
    
    return success;
}

// ============================================
// DATA UPLOAD FUNCTIONS
// ============================================

void uploadData() {
    if (!meterData.isValid()) {
        LOG_WARN("Cannot upload - data not valid");
        return;
    }
    
    LOG_INFO("─────────────────────────────────────");
    LOG_INFO("  Uploading Data to Cloud");
    LOG_INFO("─────────────────────────────────────");
    
    // Create JSON (without TOD for smaller payload)
    String jsonData = meterData.toJsonString(false);
    
    LOG_DEBUG("JSON Size: " + String(jsonData.length()) + " bytes");
    
    bool uploadSuccess = false;
    
    // Upload via MQTT
    if (MQTT_ENABLED && mqttConnected) {
        String dataTopic = String(MQTT_TOPIC_BASE) + meterData.serialNumber + "/" + MQTT_TOPIC_DATA;
        
        if (publishMQTT(dataTopic, jsonData)) {
            LOG_INFO("✓ MQTT upload successful");
            uploadSuccess = true;
        } else {
            LOG_ERROR("✗ MQTT upload failed");
        }
    }
    
    // Upload via HTTP
    if (HTTP_ENABLED && wifiConnected) {
        if (publishHTTP(jsonData)) {
            LOG_INFO("✓ HTTP upload successful");
            uploadSuccess = true;
        } else {
            LOG_ERROR("✗ HTTP upload failed");
        }
    }
    
    if (uploadSuccess) {
        HardwareManager::blinkLED(LEDColor::GREEN, 2, 200, 200);
    } else {
        LOG_WARN("No upload method succeeded");
    }
    
    LOG_INFO("─────────────────────────────────────\n");
}

// ============================================
// ERROR HANDLING
// ============================================

void handleErrors() {
    consecutiveErrors++;
    
    LOG_ERROR("Consecutive errors: " + String(consecutiveErrors));
    
    if (consecutiveErrors >= MAX_CONSECUTIVE_ERRORS) {
        LOG_ERROR("Maximum consecutive errors reached!");
        HardwareManager::showError(5);
        
        // Try recovery
        LOG_WARN("Attempting error recovery...");
        
        // Reset DLMS
        dlms.begin();
        dlms.resetErrors();
        
        // Optional: Restart if errors persist
        if (consecutiveErrors >= MAX_CONSECUTIVE_ERRORS * 2) {
            LOG_ERROR("Critical error - restarting system...");
            delay(2000);
            ESP.restart();
        }
        
        delay(ERROR_RECOVERY_DELAY);
        consecutiveErrors = 0;
    } else {
        HardwareManager::showError(consecutiveErrors);
    }
}

// ============================================
// STATUS & MONITORING
// ============================================

void printSystemStatus() {
    LOG_INFO("\n╔═══════════════════════════════════════════╗");
    LOG_INFO("║         SYSTEM STATUS REPORT              ║");
    LOG_INFO("╠═══════════════════════════════════════════╣");
    
    // System info
    LOG_INFO("║ Firmware: v" FIRMWARE_VERSION "                         ║");
    LOG_INFO("║ Uptime: " + String(millis() / 1000 / 60) + " minutes                      ║");
    LOG_INFO("║ Free Heap: " + String(ESP.getFreeHeap()) + " bytes            ║");
    LOG_INFO("╠═══════════════════════════════════════════╣");
    
    // WiFi status
    if (wifiConnected) {
        LOG_INFO("║ WiFi: Connected ✓                        ║");
        LOG_INFO("║ IP: " + WiFi.localIP().toString() + "                  ║");
        LOG_INFO("║ RSSI: " + String(WiFi.RSSI()) + " dBm                       ║");
    } else {
        LOG_INFO("║ WiFi: Disconnected ✗                     ║");
    }
    LOG_INFO("╠═══════════════════════════════════════════╣");
    
    // MQTT status
    if (mqttConnected) {
        LOG_INFO("║ MQTT: Connected ✓                        ║");
    } else {
        LOG_INFO("║ MQTT: Disconnected ✗                     ║");
    }
    LOG_INFO("╠═══════════════════════════════════════════╣");
    
    // Meter status
    LOG_INFO("║ Meter S/N: " + meterData.serialNumber + "                   ║");
    LOG_INFO("║ Readings: " + String(readingCount) + "                           ║");
    LOG_INFO("║ Errors: " + String(consecutiveErrors) + "/" + String(MAX_CONSECUTIVE_ERRORS) + "                           ║");
    LOG_INFO("║ Data Valid: " + String(meterData.isValid() ? "Yes ✓" : "No ✗") + "                  ║");
    
    if (meterData.isValid()) {
        LOG_INFO("║ kWh: " + String(meterData.kwhImport, 2) + "                         ║");
        LOG_INFO("║ Voltage: " + String(meterData.voltageR, 0) + "V                        ║");
        LOG_INFO("║ Current: " + String(meterData.currentR, 2) + "A                       ║");
    }
    
    LOG_INFO("╚═══════════════════════════════════════════╝\n");
}

// ============================================
// OPTIONAL: THINGSPEAK UPLOAD
// ============================================

#if THINGSPEAK_ENABLED
void uploadToThingSpeak() {
    if (!wifiConnected) return;
    
    HTTPClient http;
    String url = "http://api.thingspeak.com/update?api_key=" + String(THINGSPEAK_API_KEY);
    url += "&field1=" + String(meterData.kwhImport);
    url += "&field2=" + String(meterData.voltageR);
    url += "&field3=" + String(meterData.currentR);
    url += "&field4=" + String(meterData.powerFactor);
    url += "&field5=" + String(meterData.frequency);
    url += "&field6=" + String(meterData.mdKWImport.value);
    
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
        LOG_INFO("ThingSpeak upload successful");
    } else {
        LOG_ERROR("ThingSpeak upload failed");
    }
    
    http.end();
}
#endif

// ============================================
// OPTIONAL: WEB SERVER (if enabled)
// ============================================

#if ENABLE_WEB_SERVER
#include <WebServer.h>

WebServer webServer(80);

void setupWebServer() {
    webServer.on("/", []() {
        String html = "<html><body>";
        html += "<h1>DLMS Meter Reader</h1>";
        html += "<p>Serial: " + meterData.serialNumber + "</p>";
        html += "<p>kWh: " + String(meterData.kwhImport) + "</p>";
        html += "<p>Voltage: " + String(meterData.voltageR) + " V</p>";
        html += "<p><a href='/data'>JSON Data</a></p>";
        html += "</body></html>";
        webServer.send(200, "text/html", html);
    });
    
    webServer.on("/data", []() {
        String json = meterData.toJsonString(true);
        webServer.send(200, "application/json", json);
    });
    
    webServer.begin();
    LOG_INFO("Web server started on port 80");
}

void handleWebServer() {
    webServer.handleClient();
}
#endif
