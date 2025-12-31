# ESP32 Relay Controller for Exhibit Display

A web-based relay controller for interactive museum/exhibit displays using an ESP32 microcontroller. Controls 8 relay channels with a mobile-friendly web interface and automatic cycling capabilities.

---

## Table of Contents

1. [How It Works](#how-it-works)
2. [Features Explained](#features-explained)
3. [Hardware Requirements](#hardware-requirements)
4. [Installation](#installation)
5. [Configuration Guide](#configuration-guide)
6. [Usage](#usage)
7. [API Endpoints](#api-endpoints)
8. [Troubleshooting](#troubleshooting)

---

## How It Works

### The Exhibit Devices

This controller manages 8 exhibit devices that demonstrate different energy concepts:

| Relay # | Device | What It Shows | Duration |
|---------|--------|---------------|----------|
| 1 | Incandescent Light | Traditional inefficient lighting | 10 sec |
| 2 | CFL Light | Compact fluorescent - more efficient | 10 sec |
| 3 | LED Light | Most efficient lighting | 10 sec |
| 4 | Lava Lamp | Ambient/decorative energy use | 63 sec |
| 5 | Hair Dryer | High-power appliance | 7 sec |
| 6 | Mini-Fridge | Continuous-use appliance | 30 sec |
| 7 | Meters & Data | Energy monitoring display | 70 sec |
| 8 | Geothermal | Renewable energy demonstration | 86 sec |

---

### Auto-Cycle Mode

When no one is interacting, the system automatically cycles through all exhibits:

```
┌─────────────────────────────────────────────────────────────────────┐
│                        AUTO-CYCLE SEQUENCE                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   START                                                             │
│     │                                                               │
│     ▼                                                               │
│   Relay 1 (Incandescent) ON for 10 sec → OFF                       │
│     │                                                               │
│     ▼                                                               │
│   Relay 2 (CFL) ON for 10 sec → OFF                                │
│     │                                                               │
│     ▼                                                               │
│   Relay 3 (LED) ON for 10 sec → OFF                                │
│     │                                                               │
│     ▼                                                               │
│   ... continues through all 8 relays ...                           │
│     │                                                               │
│     ▼                                                               │
│   Relay 8 (Geothermal) ON for 86 sec → OFF                         │
│     │                                                               │
│     └──────────► RESTART from Relay 1 (loops forever)              │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

**Cycle Rules:**
- Only ONE relay is active at a time
- Each relay stays ON for its configured duration
- When one relay turns OFF, the next one turns ON immediately
- After Relay 8 completes, the cycle restarts from Relay 1
- Runs continuously until a visitor presses a button

---

### Button Behavior

When a visitor presses a button on the web interface:

```
┌─────────────────────────────────────────────────────────────────────┐
│                    WHAT HAPPENS WHEN YOU PRESS A BUTTON             │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   EXAMPLE: Visitor presses "Hair Dryer" button                     │
│                                                                     │
│   Step 1: Whatever relay is currently ON → turns OFF               │
│   Step 2: Auto-cycle PAUSES                                        │
│   Step 3: Hair Dryer (Relay 5) turns ON                            │
│   Step 4: Hair Dryer stays ON for 7 seconds                        │
│   Step 5: Hair Dryer turns OFF                                     │
│   Step 6: Auto-cycle RESUMES from Relay 6 (Mini-Fridge)            │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

| Action | What Happens |
|--------|--------------|
| Press any button | Stops current relay, activates the one you pressed |
| Press same button again | Restarts that relay's timer from the beginning |
| Don't press anything | Auto-cycle continues on its own |
| Press during night mode | Only Lava Lamp works, other buttons do nothing |

---

### Button States (Visual Feedback)

The web interface shows what's happening with colors:

| Button Appearance | Meaning |
|-------------------|---------|
| 🟢 **Green / Lit Up** | This relay is currently ON |
| ⚫ **Gray / Normal** | Relay is OFF, tap to activate |
| 🔴 **Red / Disabled** | Button unavailable (night mode) |

---

## Features Explained

### 1. Web Interface

A mobile-friendly control panel that works on any device with a browser:

- **8 Large Buttons**: One for each exhibit device
- **Real-Time Status**: Buttons change color to show which relay is ON
- **Responsive Design**: Works on phones, tablets, and computers
- **No App Required**: Just open a web browser and go to the IP address

---

### 2. Auto-Cycle Mode

The system automatically rotates through all exhibits when no one is pressing buttons:

- **Unattended Operation**: Perfect for when staff aren't available
- **Continuous Loop**: Cycles through all 8 devices forever
- **Configurable Timing**: Each device has its own ON duration
- **Interrupt Anytime**: Pressing a button takes over control

---

### 3. Night Mode (Energy Saving)

During off-hours, the system saves energy by only running the Lava Lamp:

```
┌─────────────────────────────────────────────────────────────────────┐
│                          NIGHT MODE                                 │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   Default Hours: 12:00 AM (midnight) to 6:00 AM                    │
│                                                                     │
│   During Night Mode:                                                │
│   ✓ Lava Lamp (Relay 4) runs on its own cycle                      │
│   ✗ All other relays are OFF and skipped                           │
│   ✗ Web buttons for other relays are disabled                      │
│   ✓ Saves electricity during closed hours                          │
│                                                                     │
│   At 6:01 AM: Normal mode resumes, all relays cycle again          │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

### 4. Dual WiFi Mode

The ESP32 connects to your WiFi network, but has a backup plan:

| Mode | When It Happens | How to Connect |
|------|-----------------|----------------|
| **Station Mode** | WiFi connection successful | Go to the IP shown in Serial Monitor |
| **Access Point Mode** | WiFi connection failed | Connect to `ExhibitControls-XXXX` network, go to `192.168.4.1` |

This means the exhibit ALWAYS works, even if your main WiFi is down.

---

### 5. Redirect URL Feature

This feature lets you send visitors to another webpage after they interact:

**What It Does:**
- After a set time, the browser automatically goes to a different URL
- Useful for directing visitors to more information, surveys, or other exhibits

**How to Use It:**

Add these parameters to the URL:

| Parameter | What It Does | Example |
|-----------|--------------|---------|
| `timeout` | Seconds to wait before redirecting | `?timeout=30` |
| `redirectURL` | The webpage to go to | `?redirectURL=https://example.com` |

**Full Example:**
```
http://192.168.1.100/?timeout=60&redirectURL=https://environmentaldashboard.org
```

This means:
1. Visitor opens the relay controller page
2. They can press buttons and interact normally
3. After 60 seconds of the page being open, browser redirects to environmentaldashboard.org

**Use Cases:**
- Send visitors to a feedback survey after using the exhibit
- Link to educational content about energy savings
- Redirect to the main exhibit kiosk homepage
- Chain multiple exhibits together in a guided tour

---

### 6. Time Synchronization (NTP)

The ESP32 automatically gets the correct time from the internet:

- **Automatic**: No manual clock setting required
- **Accurate**: Syncs with official time servers
- **Required For**: Night mode to work correctly
- **Timezone Aware**: Configurable for your location

---

## Hardware Requirements

| Component | Specification |
|-----------|---------------|
| Microcontroller | ESP32 Development Board |
| Relay Module | 8-Channel Relay Module (Active LOW) |
| Power Supply | 5V USB or appropriate power adapter |
| Wiring | Jumper wires for GPIO connections |

### GPIO Pin Mapping

| Relay # | Device | GPIO Pin |
|---------|--------|----------|
| 1 | Incandescent | GPIO 4 |
| 2 | CFL | GPIO 5 |
| 3 | LED | GPIO 18 |
| 4 | Lava Lamp | GPIO 19 |
| 5 | Hair Dryer | GPIO 21 |
| 6 | Mini-Fridge | GPIO 22 |
| 7 | Meters & Data | GPIO 23 |
| 8 | Geothermal | GPIO 25 |

### Wiring Diagram

```
                    ┌─────────────────┐
                    │      ESP32      │
                    │                 │
  Relay 1 (IN1) ←───┤ GPIO 4      5V ├───→ Relay Module VCC
  Relay 2 (IN2) ←───┤ GPIO 5     GND ├───→ Relay Module GND
  Relay 3 (IN3) ←───┤ GPIO 18        │
  Relay 4 (IN4) ←───┤ GPIO 19        │
  Relay 5 (IN5) ←───┤ GPIO 21        │
  Relay 6 (IN6) ←───┤ GPIO 22        │
  Relay 7 (IN7) ←───┤ GPIO 23        │
  Relay 8 (IN8) ←───┤ GPIO 25        │
                    └─────────────────┘
```

---

## Installation

### Step 1: Install Arduino IDE

Download from: https://www.arduino.cc/en/software

### Step 2: Install ESP32 Board Support

1. Open Arduino IDE
2. Go to `File` → `Preferences`
3. Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to `Tools` → `Board` → `Boards Manager`
5. Search "ESP32" and install "esp32 by Espressif Systems"

### Step 3: Configure Upload Settings

| Setting | Value |
|---------|-------|
| Board | ESP32 Dev Module |
| Upload Speed | 921600 |
| Flash Frequency | 80MHz |
| Partition Scheme | Default 4MB with spiffs |

### Step 4: Upload the Code

1. Connect ESP32 via USB
2. Select the correct Port in `Tools` → `Port`
3. Click Upload
4. **If upload fails**: Hold BOOT button on ESP32 during "Connecting..."

### Step 5: Upload the GIF Image (Optional)

1. Install "ESP32 Sketch Data Upload" tool for Arduino IDE
2. Create a `data` folder in your sketch folder
3. Place `ElectricityButton.gif` in the `data` folder
4. Use `Tools` → `ESP32 Sketch Data Upload`

---

## Configuration Guide

### 1. WiFi Credentials

```cpp
#define STASSID "YourNetworkName"     // Your WiFi name
#define STAPSK "YourPassword"          // Your WiFi password
```

---

### 2. Relay Durations

How long each relay stays ON (in milliseconds):

```cpp
unsigned long onDurations[] = {
  10000,   // Relay 1: 10 seconds
  10000,   // Relay 2: 10 seconds
  10000,   // Relay 3: 10 seconds
  63000,   // Relay 4: 63 seconds
  7000,    // Relay 5: 7 seconds
  30000,   // Relay 6: 30 seconds
  70000,   // Relay 7: 70 seconds
  86000    // Relay 8: 86 seconds
};
```

**Time Conversion:** `seconds × 1000 = milliseconds`

| Time | Milliseconds |
|------|--------------|
| 10 seconds | 10000 |
| 30 seconds | 30000 |
| 1 minute | 60000 |
| 2 minutes | 120000 |

---

### 3. Night Mode Hours

```cpp
const int lavaOnlyStartHour = 0;   // Start: midnight (0:00)
const int lavaOnlyEndHour = 6;     // End: 6:00 AM
```

**To disable night mode:** Set both values to `0`.

---

### 4. Timezone

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

### 5. Access Point Password

```cpp
const char* apPassword = "exhibit123";
```

Network name will be: `ExhibitControls-XXXX`

---

### 6. GPIO Pins

```cpp
const int relayPins[] = {4, 5, 18, 19, 21, 22, 23, 25};
```

**Avoid these pins:** GPIO 0, 2, 12 (boot pins) and 34-39 (input only)

---

### 7. Button Labels

To change button text in the web interface:

```cpp
"<button onclick=\"controlRelay(1)\">1. Light-Incandescent</button>"
```

Change the text between `>` and `</button>`.

---

## Usage

### Accessing the Controller

| Method | Address |
|--------|---------|
| Via WiFi | `http://[IP from Serial Monitor]` |
| Via Access Point | Connect to `ExhibitControls-XXXX`, go to `http://192.168.4.1` |
| Via mDNS | `http://exhibit-XXXX.local` |

### Using Redirect Feature

```
http://192.168.1.100/?timeout=60&redirectURL=https://yoursite.com
```

| Parameter | Required | Description |
|-----------|----------|-------------|
| `timeout` | Yes (for redirect) | Seconds before redirect |
| `redirectURL` | Yes (for redirect) | Where to send the visitor |

---

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Main web interface |
| `/relay?num=X` | GET | Activate relay X (1-8) |
| `/relayStates` | GET | JSON array of relay states |

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Upload fails | Hold BOOT button during "Connecting..." |
| WiFi won't connect | Check SSID/password, use 2.4GHz network |
| Relays not switching | Check wiring, verify active-low relay module |
| Web page not loading | Check IP in Serial Monitor, same network? |
| Night mode not working | Verify timezone is correct |
| Wrong time displayed | Check NTP connectivity, timezone string |
| Redirect not working | Both `timeout` and `redirectURL` required |

---

## License

Open source for educational and non-commercial use.

---

## Contributing

Contributions welcome! Submit issues or pull requests to improve this project.
