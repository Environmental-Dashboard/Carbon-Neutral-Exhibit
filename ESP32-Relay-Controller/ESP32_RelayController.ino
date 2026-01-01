/*
 * ============================================================================
 * ESP32 RELAY CONTROLLER FOR EXHIBIT DISPLAY
 * ============================================================================
 * 
 * DESCRIPTION:
 * This controller manages 8 relay channels for an interactive exhibit display.
 * It provides both a web interface and automatic cycling through exhibits.
 * 
 * HARDWARE REQUIREMENTS:
 * - ESP32 Development Board
 * - 8-Channel Relay Module (Active LOW)
 * - USB cable for programming and power
 * 
 * ============================================================================
 * CONFIGURABLE PARAMETERS - EDIT THESE TO CUSTOMIZE BEHAVIOR
 * ============================================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <time.h>

/* ---------------------------------------------------------------------------
 * WIFI CONFIGURATION
 * Change these to match your network credentials
 * ---------------------------------------------------------------------------*/
#ifndef STASSID
#define STASSID "ObieConnect"     // <-- CHANGE THIS: Your Wi-Fi network name (SSID)
#define STAPSK "122ElmStreet"     // <-- CHANGE THIS: Your Wi-Fi password
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

/* ---------------------------------------------------------------------------
 * RELAY PIN CONFIGURATION
 * Maps each relay position (1-8) to an ESP32 GPIO pin
 * 
 * Array Index:  [0]   [1]   [2]   [3]   [4]   [5]   [6]   [7]
 * Button #:      1     2     3     4     5     6     7     8
 * GPIO Pin:      4     5    18    19    21    22    23    25
 * 
 * To change pin assignments, modify the corresponding value in the array
 * ---------------------------------------------------------------------------*/
const int relayPins[] = {4, 5, 18, 19, 21, 22, 23, 25};
const int numRelays = sizeof(relayPins) / sizeof(relayPins[0]);

/* ---------------------------------------------------------------------------
 * RELAY LABELS - What each relay controls
 * Position 1: Light-Incandescent
 * Position 2: Light-CFL (Compact Fluorescent)
 * Position 3: Light-LED
 * Position 4: Lava Lamp
 * Position 5: Hair Dryer
 * Position 6: Mini-Fridge (Heat Pump)
 * Position 7: Meters & Data System (Water Metering)
 * Position 8: Geothermal Model
 * ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 * ON-DURATION CONFIGURATION (in milliseconds)
 * How long each relay stays ON when activated
 * 
 * Array Index:    [0]     [1]     [2]     [3]     [4]     [5]     [6]     [7]
 * Button #:        1       2       3       4       5       6       7       8
 * Duration(ms): 10000   10000   10000   63000    7000   30000   70000   86000
 * Duration(s):    10s     10s     10s     63s      7s     30s     70s     86s
 * 
 * CONVERSION: seconds × 1000 = milliseconds
 * Example: 1 minute 10 seconds = 70 seconds = 70000 milliseconds
 * 
 * TO CHANGE A DURATION: Modify the value at the corresponding index
 * ---------------------------------------------------------------------------*/
unsigned long onDurations[] = {
  10000,   // [0] Relay 1: Incandescent Light - 10 seconds
  10000,   // [1] Relay 2: CFL Light - 10 seconds
  10000,   // [2] Relay 3: LED Light - 10 seconds
  63000,   // [3] Relay 4: Lava Lamp - 63 seconds (1 min 3 sec)
  7000,    // [4] Relay 5: Hair Dryer - 7 seconds
  30000,   // [5] Relay 6: Mini-Fridge - 30 seconds
  70000,   // [6] Relay 7: Meters & Data - 70 seconds (1 min 10 sec)
  86000    // [7] Relay 8: Geothermal - 86 seconds (1 min 26 sec)
};

/* ---------------------------------------------------------------------------
 * LAVA LAMP NIGHT MODE CONFIGURATION
 * During these hours, ONLY the Lava Lamp will run (energy saving mode)
 * Uses 24-hour format based on local timezone
 * ---------------------------------------------------------------------------*/
const int lavaOnlyStartHour = 0;   // Start hour (0 = midnight)
const int lavaOnlyEndHour = 6;     // End hour (6 = 6:00 AM)

/* ---------------------------------------------------------------------------
 * ACCESS POINT (FALLBACK) CONFIGURATION
 * If WiFi connection fails, ESP32 creates its own network with these settings
 * ---------------------------------------------------------------------------*/
const char* apPassword = "exhibit123";  // <-- CHANGE THIS: Fallback AP password

/* ---------------------------------------------------------------------------
 * RETURN BUTTON CONFIGURATION
 * The blue button at the bottom that links back to the main display
 * ---------------------------------------------------------------------------*/
const char* returnButtonText = "Return to Carbon Neutral Stories";
const char* returnButtonURL = "https://oberlin.communityhub.cloud/digital-signage-v2-fe/remote/oc-carbon-neutral-display";

/* ---------------------------------------------------------------------------
 * TIMEZONE CONFIGURATION
 * Current setting: Eastern Time (EST/EDT) with automatic DST switching
 * Format: "STD_OFFSET DST_OFFSET, START_RULE, END_RULE"
 * 
 * Common timezone strings:
 * - Eastern:  "EST5EDT,M3.2.0/2,M11.1.0/2"
 * - Central:  "CST6CDT,M3.2.0/2,M11.1.0/2"
 * - Mountain: "MST7MDT,M3.2.0/2,M11.1.0/2"
 * - Pacific:  "PST8PDT,M3.2.0/2,M11.1.0/2"
 * ---------------------------------------------------------------------------*/
#define TIMEZONE_STRING "EST5EDT,M3.2.0/2,M11.1.0/2"

/* ============================================================================
 * END OF CONFIGURABLE PARAMETERS
 * ============================================================================
 * WARNING: Only modify code below if you understand the functionality
 * ============================================================================
 */

WebServer server(80);

// State tracking variables
volatile bool webInteraction = false;  // True when user interacts via web
volatile int selectedRelay = -1;       // Currently selected relay index
unsigned long interactionStart = 0;    // Timestamp when interaction started
bool inDefaultLoop = true;             // True when in automatic cycling mode

int lastActivatedRelay = -1;           // Last relay that was activated
char apSsid[32] = {0};                 // Generated AP name (includes MAC suffix)

/* ---------------------------------------------------------------------------
 * WEB INTERFACE HTML
 * Embedded HTML/CSS/JavaScript for the control panel
 * Button labels can be modified in the <button> tags below
 * ---------------------------------------------------------------------------*/
const char* htmlContent = "<!DOCTYPE HTML>"
                          "<html>"
                          "<head>"
                          "<title>Exhibit Controls</title>"
                          "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                          "<style>"
                          "body { font-family: Arial, sans-serif; background-color: #2c2f33; color: #fff; text-align: center; margin: 0; padding: 10px; padding-bottom: 120px; padding-top: 74px; }"
                          ".button { display: block; margin: 10px auto; padding: 16px 24px; font-size: 24px; width: 96%; background-color: #3a3f44; color: white; border: none; border-radius: 8px; cursor: pointer; box-sizing: border-box; text-align: left; overflow: hidden; white-space: nowrap; text-overflow: ellipsis; text-decoration: none;}"
                          ".active { background-color: #2ecc71; }"
                          ".return { background-color: #2ea8ff; position: fixed; left: 2%; right: 2%; bottom: 10px; text-align: center; }"
                          ".topnav { position: fixed; top: 0; left: 0; right: 0; height: 64px; background: #8b1e1e; display: flex; align-items: center; justify-content: center; padding: 0 12px; box-shadow: 0 2px 8px rgba(0,0,0,0.35); z-index: 10; }"
                          ".brand { display: flex; align-items: center; gap: 12px; }"
                          ".brand img { height: 40px; width: auto; border-radius: 6px; }"
                          ".brand-title { font-weight: 700; font-size: 20px; }"
                          "</style>"
                          "<script>"
                          "function initPage() { "
                          "  updateRelayStates();"
                          "  const params = new URLSearchParams(window.location.search);"
                          "  const timeoutParam = parseInt(params.get('timeout'));"
                          "  const redirectURL = params.get('redirectURL');"
                          "  const link = document.getElementById('returnLink');"
                          "  if (redirectURL && link) { link.href = redirectURL; }"
                          "  const targetURL = redirectURL || link.href;"
                          "  if (!isNaN(timeoutParam) && timeoutParam > 0) {"
                          "    setTimeout(function(){ window.location.href = targetURL; }, timeoutParam * 1000);"
                          "  }"
                          "}"
                          "function updateRelayStates() {"
                          "  fetch('/relayStates').then(r => r.json()).then(states => {"
                          "    for (let i = 0; i < states.length; i++) {"
                          "      const button = document.getElementById('relay' + (i + 1));"
                          "      if (states[i]) button.classList.add('active');"
                          "      else button.classList.remove('active');"
                          "    }"
                          "  });"
                          "}"
                          "setInterval(updateRelayStates, 2000);"
                          "function controlRelay(relayNum) { fetch('/relay?num=' + relayNum).then(() => updateRelayStates()); }"
                          "</script>"
                          "</head>"
                          "<body onload=\"initPage()\">"
                          "<div class=\"topnav\"><div class=\"brand\">"
                          "  <img src=\"/ElectricityButton.gif\" alt=\"Electricity Button\" />"
                          "  <span class=\"brand-title\">Select an Exhibit Case Feature:</span>"
                          "</div></div>"
                          // BUTTON LABELS - Modify text between > and </button> to change labels
                          "<button id=\"relay1\" class=\"button\" onclick=\"controlRelay(1)\">1. Light-Incandescent</button>"
                          "<button id=\"relay2\" class=\"button\" onclick=\"controlRelay(2)\">2. Light - Compact Fluorescent</button>"
                          "<button id=\"relay3\" class=\"button\" onclick=\"controlRelay(3)\">3. Light - LED</button>"
                          "<button id=\"relay4\" class=\"button\" onclick=\"controlRelay(4)\">4. Lava Lamp</button>"
                          "<button id=\"relay5\" class=\"button\" onclick=\"controlRelay(5)\">5. Hair Dryer</button>"
                          "<button id=\"relay6\" class=\"button\" onclick=\"controlRelay(6)\">6. Mini-Fridge (Heat Pump)</button>"
                          "<button id=\"relay7\" class=\"button\" onclick=\"controlRelay(7)\">7. Show Meters & Data System</button>"
                          "<button id=\"relay8\" class=\"button\" onclick=\"controlRelay(8)\">8. Show Geothermal Model</button>"
                          "<a id=\"returnLink\" class=\"button return\" href=\"https://oberlin.communityhub.cloud/digital-signage-v2-fe/remote/oc-carbon-neutral-display\">Return to Carbon Neutral Stories</a>"
                          "</body></html>";

/* ---------------------------------------------------------------------------
 * controlRelay(int relayNum)
 * Activates a single relay while turning off all others
 * 
 * @param relayNum: Relay index (0-7) to activate
 * Note: Relays are ACTIVE LOW (LOW = ON, HIGH = OFF)
 * ---------------------------------------------------------------------------*/
void controlRelay(int relayNum) {
  for (int i = 0; i < numRelays; i++) {
    digitalWrite(relayPins[i], (i == relayNum) ? LOW : HIGH);
  }
  Serial.printf("Relay %d is ON\n", relayNum + 1);
}

/* ---------------------------------------------------------------------------
 * turnOffAllRelays()
 * Turns off all relays (sets all pins HIGH)
 * ---------------------------------------------------------------------------*/
void turnOffAllRelays() {
  for (int i = 0; i < numRelays; i++) digitalWrite(relayPins[i], HIGH);
  Serial.println("All relays are OFF");
}

/* ---------------------------------------------------------------------------
 * isLavaLampOnlyTime()
 * Checks if current time is within the Lava Lamp only period
 * 
 * @return: true if current hour is between lavaOnlyStartHour and lavaOnlyEndHour
 * ---------------------------------------------------------------------------*/
bool isLavaLampOnlyTime() {
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);
  return (t->tm_hour >= lavaOnlyStartHour && t->tm_hour < lavaOnlyEndHour);
}

/* ---------------------------------------------------------------------------
 * HTTP REQUEST HANDLERS
 * ---------------------------------------------------------------------------*/

// Serves the main web interface
void handleRoot() { server.send(200, "text/html", htmlContent); }

// Handles relay control requests from web interface
void handleRelayControl() {
  if (server.hasArg("num")) {
    int relayNum = server.arg("num").toInt() - 1;  // Convert to 0-based index
    if (relayNum >= 0 && relayNum < numRelays) {
      controlRelay(relayNum);
      webInteraction = true;
      selectedRelay = relayNum;
      interactionStart = millis();
      inDefaultLoop = false;
      server.send(200, "text/plain", "Relay " + String(relayNum + 1) + " is ON");
    } else server.send(400, "text/plain", "Invalid relay number");
  } else server.send(400, "text/plain", "Relay number not specified");
}

// Returns JSON array of relay states for web interface updates
void handleRelayStates() {
  String json = "[";
  for (int i = 0; i < numRelays; i++) {
    bool isActive = false;
    if (webInteraction && selectedRelay == i) isActive = true;
    else isActive = (digitalRead(relayPins[i]) == LOW);
    json += isActive ? "true" : "false";
    if (i < numRelays - 1) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
}

// Handles 404 errors - serves main page for GET requests
void handleNotFound() {
  if (server.method() == HTTP_GET) {
    server.sendHeader("Cache-Control", "no-cache");
    server.send(200, "text/html", htmlContent);
    return;
  }
  server.send(404, "text/plain", "404: Not Found");
}

/* ---------------------------------------------------------------------------
 * setup()
 * Initializes hardware, WiFi, web server, and time synchronization
 * ---------------------------------------------------------------------------*/
void setup() {
  Serial.begin(115200);
  
  // Initialize all relay pins as outputs and turn them OFF
  for (int i = 0; i < numRelays; i++) { 
    pinMode(relayPins[i], OUTPUT); 
    digitalWrite(relayPins[i], HIGH);  // HIGH = OFF for active-low relays
  }

  // Configure WiFi in dual mode (Station + Access Point)
  WiFi.mode(WIFI_AP_STA); 
  WiFi.setSleep(false); 
  WiFi.setAutoReconnect(true); 
  WiFi.persistent(false);
  
  // Attempt to connect to WiFi network
  WiFi.begin(ssid, password);
  unsigned long wifiTimeout = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiTimeout < 15000) { 
    delay(250); 
    server.handleClient(); 
    Serial.print("."); 
  }
  
  if (WiFi.status() == WL_CONNECTED) { 
    Serial.println("\nWi-Fi connected"); 
    Serial.print("IP: "); 
    Serial.println(WiFi.localIP()); 
  } else {
    Serial.println("\nWi-Fi failed; SoftAP fallback");
  }

  // Create fallback Access Point with unique name based on MAC address
  uint8_t mac[6]; 
  WiFi.macAddress(mac); 
  snprintf(apSsid, sizeof(apSsid), "ExhibitControls-%02X%02X", mac[4], mac[5]);
  WiFi.softAP(apSsid, apPassword);

  // Configure timezone and sync time with NTP servers
  configTzTime(TIMEZONE_STRING, "pool.ntp.org", "time.nist.gov");

  // Initialize SPIFFS filesystem for serving static files
  if (!SPIFFS.begin(true)) Serial.println("SPIFFS Mount Failed");

  // Setup mDNS for easy access (e.g., http://exhibit-XXXX.local)
  char mdnsName[32]; 
  snprintf(mdnsName, sizeof(mdnsName), "exhibit-%02X%02X", mac[4], mac[5]);
  MDNS.begin(mdnsName);

  // Register HTTP request handlers
  server.on("/", handleRoot);
  server.on("/relay", handleRelayControl);
  server.on("/relayStates", handleRelayStates);
  server.onNotFound(handleNotFound);

  // Serve the electricity button GIF from SPIFFS
  server.on("/ElectricityButton.gif", HTTP_GET, []() {
    File file = SPIFFS.open("/ElectricityButton.gif", "r");
    if (file) { 
      server.sendHeader("Cache-Control", "public, max-age=86400"); 
      server.streamFile(file, "image/gif"); 
      file.close(); 
    } else {
      server.send(404, "text/plain", "File not found");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
  
  }

/* ---------------------------------------------------------------------------
 * loop()
 * Main program loop - handles web requests and automatic relay cycling
 * 
 * OPERATION MODES:
 * 1. Lava Lamp Only Mode: During night hours, only Lava Lamp runs
 * 2. Web Interaction Mode: When user selects a relay via web interface
 * 3. Auto Cycle Mode: Automatically cycles through all relays
 * ---------------------------------------------------------------------------*/
void loop() {
  server.handleClient();

  // NIGHT MODE: Only run Lava Lamp during configured hours
  if (isLavaLampOnlyTime()) {
    controlRelay(3);  // Lava Lamp = relay index 3 (button #4)
    delay(10);
    return;
  }

  // WEB INTERACTION MODE: User selected a relay via web interface
  if (webInteraction) {
    if (!inDefaultLoop) { 
      controlRelay(selectedRelay); 
      inDefaultLoop = true; 
    }
    // Check if the on-duration has elapsed
    if (millis() - interactionStart > onDurations[selectedRelay]) {
      webInteraction = false; 
      inDefaultLoop = true; 
      turnOffAllRelays(); 
      lastActivatedRelay = selectedRelay;
    }
    return;
  }

  // AUTO CYCLE MODE: Automatically cycle through relays
  if (inDefaultLoop && !webInteraction) {
    for (int i = (lastActivatedRelay + 1) % numRelays; i < numRelays; i++) {
      controlRelay(i); 
      lastActivatedRelay = i; 
      unsigned long startMillis = millis();
      
      // Keep relay on for its configured duration
      while (millis() - startMillis < onDurations[i]) {
        server.handleClient(); 
        delay(1);
        // Exit if user interacts or night mode activates
        if (webInteraction || isLavaLampOnlyTime()) { 
          controlRelay(3); 
          return; 
        }
      }
    }
    lastActivatedRelay = -1;  // Reset to start from beginning
  }
}
