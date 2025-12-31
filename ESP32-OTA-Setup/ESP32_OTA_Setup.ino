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

/* ============================================================================
 * END OF CONFIGURATION
 * ============================================================================
 */

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nESP32 OTA Setup Starting...");
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 60) {  // 30 second timeout
      Serial.println("\nWiFi connection failed! Check credentials.");
      Serial.println("Restarting in 5 seconds...");
      delay(5000);
      ESP.restart();
    }
  }
  
  Serial.println("\n✓ WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Setup OTA
  ArduinoOTA.setHostname(otaHostname);
  
  // Optional: Set OTA password for security
  // ArduinoOTA.setPassword("admin");
  
  // OTA callbacks for status feedback
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("OTA Update Starting: " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\n✓ OTA Update Complete!");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  
  Serial.println("\n============================================");
  Serial.println("OTA UPDATE READY!");
  Serial.println("============================================");
  Serial.print("Hostname: ");
  Serial.println(otaHostname);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("\nIn Arduino IDE:");
  Serial.println("1. Go to Tools → Port");
  Serial.println("2. Look for: " + String(otaHostname) + " at " + WiFi.localIP().toString());
  Serial.println("3. Select it and upload any sketch wirelessly!");
  Serial.println("============================================\n");
}

void loop() {
  // Handle OTA updates
  ArduinoOTA.handle();
  
  // Keep WiFi alive
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    delay(5000);
  }
}

