# ESP32 Relay Controller for Exhibit Display

A web-based relay controller for interactive museum/exhibit displays using an ESP32 microcontroller. Controls 8 relay channels with a mobile-friendly web interface and automatic cycling capabilities.

## Features

- **Web Interface**: Mobile-responsive control panel accessible from any browser
- **8 Relay Channels**: Independent control of 8 devices/exhibits
- **Auto-Cycle Mode**: Automatically rotates through exhibits when idle
- **Night Mode**: Energy-saving mode that only runs the Lava Lamp during configurable hours
- **Dual WiFi Mode**: Connects to existing network or creates its own access point as fallback
- **Time Sync**: Automatic NTP time synchronization for scheduled operations

---

## Hardware Requirements

| Component | Specification |
|-----------|---------------|
| Microcontroller | ESP32 Development Board |
| Relay Module | 8-Channel Relay Module (Active LOW) |
| Power Supply | 5V USB or appropriate power adapter |
| Wiring | Jumper wires for GPIO connections |

### GPIO Pin Mapping

| Relay # | Button Label | GPIO Pin | Default Duration |
|---------|-------------|----------|------------------|
| 1 | Light-Incandescent | GPIO 4 | 10 seconds |
| 2 | Light-CFL | GPIO 5 | 10 seconds |
| 3 | Light-LED | GPIO 18 | 10 seconds |
| 4 | Lava Lamp | GPIO 19 | 63 seconds |
| 5 | Hair Dryer | GPIO 21 | 7 seconds |
| 6 | Mini-Fridge | GPIO 22 | 30 seconds |
| 7 | Meters & Data | GPIO 23 | 70 seconds |
| 8 | Geothermal | GPIO 25 | 86 seconds |

---

## Configuration Guide

### 1. WiFi Settings

Located at the top of `ESP32_RelayController.ino`:

```cpp
#define STASSID "ObieConnect"     // Your WiFi network name
#define STAPSK "122ElmStreet"     // Your WiFi password
```

**To change:** Replace the values in quotes with your network credentials.

---

### 2. Relay On-Durations

Controls how long each relay stays ON when activated. Values are in **milliseconds**.

```cpp
unsigned long onDurations[] = {
  10000,   // [0] Relay 1: Incandescent Light - 10 seconds
  10000,   // [1] Relay 2: CFL Light - 10 seconds
  10000,   // [2] Relay 3: LED Light - 10 seconds
  63000,   // [3] Relay 4: Lava Lamp - 63 seconds
  7000,    // [4] Relay 5: Hair Dryer - 7 seconds
  30000,   // [5] Relay 6: Mini-Fridge - 30 seconds
  70000,   // [6] Relay 7: Meters & Data - 70 seconds
  86000    // [7] Relay 8: Geothermal - 86 seconds
};
```

#### Time Conversion Formula

```
seconds × 1000 = milliseconds
```

| Desired Time | Calculation | Value |
|--------------|-------------|-------|
| 10 seconds | 10 × 1000 | 10000 |
| 30 seconds | 30 × 1000 | 30000 |
| 1 minute | 60 × 1000 | 60000 |
| 1 min 10 sec | 70 × 1000 | 70000 |
| 2 minutes | 120 × 1000 | 120000 |

---

### 3. GPIO Pin Assignments

If you need to use different pins on the ESP32:

```cpp
const int relayPins[] = {4, 5, 18, 19, 21, 22, 23, 25};
//                       ^  ^  ^   ^   ^   ^   ^   ^
//                       |  |  |   |   |   |   |   Relay 8
//                       |  |  |   |   |   |   Relay 7
//                       |  |  |   |   |   Relay 6
//                       |  |  |   |   Relay 5
//                       |  |  |   Relay 4
//                       |  |  Relay 3
//                       |  Relay 2
//                       Relay 1
```

**Note:** Use ESP32 GPIO pins that support output. Avoid GPIO 0, 2, 12 (boot pins) and 34-39 (input only).

---

### 4. Night Mode (Lava Lamp Only Hours)

During these hours, only the Lava Lamp will run to save energy:

```cpp
const int lavaOnlyStartHour = 0;   // Start: midnight (0:00)
const int lavaOnlyEndHour = 6;     // End: 6:00 AM
```

**To disable night mode:** Set both values to 0.

---

### 5. Timezone Configuration

```cpp
#define TIMEZONE_STRING "EST5EDT,M3.2.0/2,M11.1.0/2"
```

| Timezone | String |
|----------|--------|
| Eastern | `"EST5EDT,M3.2.0/2,M11.1.0/2"` |
| Central | `"CST6CDT,M3.2.0/2,M11.1.0/2"` |
| Mountain | `"MST7MDT,M3.2.0/2,M11.1.0/2"` |
| Pacific | `"PST8PDT,M3.2.0/2,M11.1.0/2"` |

---

### 6. Access Point Fallback

If WiFi connection fails, ESP32 creates its own network:

```cpp
const char* apPassword = "exhibit123";  // Fallback AP password
```

The network name will be: `ExhibitControls-XXXX` (where XXXX is part of the MAC address)

---

### 7. Button Labels in Web Interface

To change the button text shown in the web interface, find this section:

```cpp
"<button id=\"relay1\" class=\"button\" onclick=\"controlRelay(1)\">1. Light-Incandescent</button>"
"<button id=\"relay2\" class=\"button\" onclick=\"controlRelay(2)\">2. Light - Compact Fluorescent</button>"
// ... etc
```

Modify the text between `>` and `</button>` for each button.

---

## Installation

### Arduino IDE Setup

1. **Install ESP32 Board Support**:
   - Go to `File` → `Preferences`
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to `Tools` → `Board` → `Boards Manager`
   - Search "ESP32" and install "esp32 by Espressif Systems"

2. **Select Board**:
   - `Tools` → `Board` → `ESP32 Dev Module`

3. **Upload Settings**:
   - Upload Speed: 921600
   - Flash Frequency: 80MHz
   - Partition Scheme: Default 4MB with spiffs

4. **Upload the Code**:
   - Connect ESP32 via USB
   - Select the correct Port in `Tools` → `Port`
   - Click Upload
   - If upload fails, hold BOOT button on ESP32 during "Connecting..."

### SPIFFS File Upload (for GIF image)

1. Install "ESP32 Sketch Data Upload" tool for Arduino IDE
2. Create a `data` folder in your sketch folder
3. Place `ElectricityButton.gif` in the `data` folder
4. Use `Tools` → `ESP32 Sketch Data Upload`

---

## Usage

### Accessing the Web Interface

1. **Via WiFi Network**: 
   - Check Serial Monitor for IP address (e.g., `192.168.1.100`)
   - Open browser and go to `http://[IP_ADDRESS]`

2. **Via Access Point** (if WiFi fails):
   - Connect to WiFi network `ExhibitControls-XXXX`
   - Password: `exhibit123`
   - Go to `http://192.168.4.1`

3. **Via mDNS** (if supported by your device):
   - Go to `http://exhibit-XXXX.local`

### URL Parameters

The web interface supports these URL parameters:

| Parameter | Description | Example |
|-----------|-------------|---------|
| `timeout` | Auto-redirect after N seconds | `?timeout=30` |
| `redirectURL` | URL to redirect to | `?redirectURL=https://example.com` |

Example: `http://192.168.1.100/?timeout=60&redirectURL=https://exhibit-main.com`

---

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Main web interface |
| `/relay?num=X` | GET | Activate relay X (1-8) |
| `/relayStates` | GET | JSON array of relay states |

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Upload fails | Hold BOOT button during "Connecting..." |
| WiFi won't connect | Check SSID/password, ensure 2.4GHz network |
| Relays not switching | Check wiring, verify active-low relay module |
| Web page not loading | Check IP in Serial Monitor, verify same network |
| Time-based features wrong | Verify timezone string is correct |

---

## License

This project is open source and available for educational and non-commercial use.

---

## Contributing

Contributions are welcome! Please submit issues or pull requests to improve this project.
