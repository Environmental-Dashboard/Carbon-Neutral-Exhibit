/*
 * ============================================================================
 * ESP32 OTA (Over-The-Air) UPDATE SETUP
 * ============================================================================
 * 
 * PURPOSE:
 * Upload this sketch to ALL ESP32s ONCE via USB. After that, you can upload
 * new code wirelessly via WiFi - no USB cable needed!
 * 
 * HOW IT WORKS:
 * 1. Upload this sketch via USB (first time only)
 * 2. ESP32 connects to WiFi and enables OTA updates
 * 3. In Arduino IDE: Tools → Port → select the ESP32's network address
 * 4. Upload any sketch wirelessly!
 * 
 * ============================================================================
 * CONFIGURATION - Change these to match your network
 * ============================================================================
 */

#include <WiFi.h>
#include <ArduinoOTA.h>

// WiFi credentials - CHANGE THESE to match your network
const char* ssid = "ObieConnect";           // <-- Your WiFi network name
const char* password = "122ElmStreet";      // <-- Your WiFi password

// OTA hostname - This name appears in Arduino IDE port menu
// Change this to identify different ESP32s (e.g., "esp32-relay", "esp32-led")
const char* otaHostname = "esp32-exhibit";

// OTA password for security
const char* otaPassword = "132ElmStreet";

/* ============================================================================
 * END OF CONFIGURATION
 * ============================================================================
 */

void setup() {
  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection (30 second timeout)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 60) {
    delay(500);
    attempts++;
  }
  
  // Restart if WiFi failed
  if (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }
  
  // Setup OTA
  ArduinoOTA.setHostname(otaHostname);
  ArduinoOTA.setPassword(otaPassword);
  ArduinoOTA.begin();
}

void loop() {
  // Handle OTA updates
  ArduinoOTA.handle();
  
  // Reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(5000);
  }
}
