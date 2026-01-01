# ESP32 Relay Controller for Exhibit Display

## Purpose

This device controls 8 electrical appliances in a museum exhibit. A separate electricity meter measures power consumption. **The goal is to let visitors see how many watts different appliances use** when turned on - comparing an incandescent bulb vs LED, or a hair dryer vs a mini-fridge.

---

## Table of Contents

1. [What This Device Does](#what-this-device-does)
2. [How the Cycle Works](#how-the-cycle-works)
3. [The Web Interface](#the-web-interface)
4. [All Configurable Parameters](#all-configurable-parameters)
5. [Hardware Setup](#hardware-setup)
6. [Software Installation](#software-installation)
7. [Troubleshooting](#troubleshooting)

---

## What This Device Does

### The Appliances

| Relay | Appliance | Why It's Included | Default ON Time |
|-------|-----------|-------------------|-----------------|
| 1 | Incandescent Light | High energy use for lighting | 10 seconds |
| 2 | CFL Light | Medium energy - more efficient | 10 seconds |
| 3 | LED Light | Low energy - most efficient | 10 seconds |
| 4 | Lava Lamp | Decorative/ambient energy use | 63 seconds |
| 5 | Hair Dryer | High-power heating appliance | 7 seconds |
| 6 | Mini-Fridge | Continuous-use appliance | 30 seconds |
| 7 | Meters & Data | Energy monitoring display | 70 seconds |
| 8 | Geothermal | Renewable energy demo | 86 seconds |

### What Visitors Experience

1. **Without interaction**: Appliances turn on one at a time, cycling automatically
2. **With interaction**: Visitor presses a button → that appliance turns on → they watch the power meter
3. **The learning**: Visitors SEE the wattage difference between appliances on the meter display

---

## How the Cycle Works

### The Basic Loop

The system runs in one continuous loop:

```
┌─────────────────────────────────────────────────────────────────────┐
│                         THE MAIN LOOP                               │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   DO FOREVER:                                                       │
│   │                                                                 │
│   ├──► Relay 1 ON for onduration1 → OFF                            │
│   ├──► Relay 2 ON for onduration2 → OFF                            │
│   ├──► Relay 3 ON for onduration3 → OFF                            │
│   ├──► Relay 4 ON for onduration4 → OFF                            │
│   ├──► Relay 5 ON for onduration5 → OFF                            │
│   ├──► Relay 6 ON for onduration6 → OFF                            │
│   ├──► Relay 7 ON for onduration7 → OFF                            │
│   ├──► Relay 8 ON for onduration8 → OFF                            │
│   │                                                                 │
│   └──► REPEAT FROM RELAY 1                                         │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

**Key Rules:**
- Only ONE relay is ON at any time
- Each relay has its own configurable ON duration
- The loop runs forever with no gaps between relays

---

### What Happens When Someone Presses a Button

```
┌─────────────────────────────────────────────────────────────────────┐
│              BUTTON PRESS BEHAVIOR                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   EXAMPLE: System is on Relay 2, visitor presses "Lava Lamp"       │
│                                                                     │
│   1. Relay 2 immediately turns OFF                                 │
│   2. Relay 4 (Lava Lamp) turns ON                                  │
│   3. Lava Lamp stays ON for onduration4 (63 seconds)               │
│   4. If no more button presses:                                    │
│      → Lava Lamp turns OFF                                         │
│      → Relay 5 turns ON (continues from where button was pressed)  │
│      → Normal cycle resumes: 5 → 6 → 7 → 8 → 1 → 2 → ...          │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

**Button Press Rules:**
| Action | What Happens |
|--------|--------------|
| Press any button | Jumps directly to that relay in the loop |
| Press button while that relay is ON | Restarts its timer |
| Stop interacting | After the selected relay finishes, loop continues from next relay |
| Press different button | Current relay OFF, new relay ON |

---

### Night Mode (Global OFF Period)

To save energy during closed hours, the system can turn ALL relays off:

```
┌─────────────────────────────────────────────────────────────────────┐
│                       NIGHT MODE                                    │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   Example: Turn off from midnight to 6:00 AM                       │
│                                                                     │
│   11:59 PM → Normal cycling (relays turning on/off)                │
│   12:00 AM → ALL relays turn OFF                                   │
│   12:01 AM → Still all OFF                                         │
│     ...                                                             │
│   5:59 AM  → Still all OFF                                         │
│   6:00 AM  → Normal cycling RESUMES                                │
│                                                                     │
│   During OFF period:                                                │
│   • All relays stay OFF                                            │
│   • Web buttons are disabled                                       │
│   • Only Lava Lamp can run (optional - for ambiance)               │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## The Web Interface

### How to Access It

The ESP32 hosts a website. Here's how to find it:

| Method | What to Do |
|--------|------------|
| **Check Serial Monitor** | Connect ESP32 to computer, open Serial Monitor at 115200 baud, look for the IP address (e.g., `192.168.1.100`) |
| **Open in Browser** | Go to `http://[IP ADDRESS]` on any phone, tablet, or computer on the same WiFi |
| **If WiFi Fails** | ESP32 creates its own network called `ExhibitControls-XXXX`. Connect to it, then go to `http://192.168.4.1` |

### What the Interface Looks Like

```
┌─────────────────────────────────────────────────────────────────────┐
│                                                                     │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │           1. Light - Incandescent                           │  │
│   └─────────────────────────────────────────────────────────────┘  │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │           2. Light - CFL                                    │  │
│   └─────────────────────────────────────────────────────────────┘  │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │           3. Light - LED                                    │  │
│   └─────────────────────────────────────────────────────────────┘  │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │           4. Lava Lamp                                      │  │
│   └─────────────────────────────────────────────────────────────┘  │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │           5. Hair Dryer                                     │  │
│   └─────────────────────────────────────────────────────────────┘  │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │           6. Mini-Fridge                                    │  │
│   └─────────────────────────────────────────────────────────────┘  │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │           7. Meters & Data                                  │  │
│   └─────────────────────────────────────────────────────────────┘  │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │           8. Geothermal                                     │  │
│   └─────────────────────────────────────────────────────────────┘  │
│                                                                     │
│   ┌─────────────────────────────────────────────────────────────┐  │
│   │        🔵 Return to Carbon Neutral Stories                  │  │
│   └─────────────────────────────────────────────────────────────┘  │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### Button States

| Button Color | Meaning |
|--------------|---------|
| **Green / Highlighted** | This relay is currently ON |
| **Gray / Normal** | Relay is OFF - tap to turn it on |
| **Disabled** | Night mode active - buttons don't work |

---

### The "Return to Carbon Neutral Stories" Button

At the bottom of the interface is a blue button:

**What it does:** Links back to the main Carbon Neutral Stories digital signage:
```
https://oberlin.communityhub.cloud/digital-signage-v2-fe/remote/oc-carbon-neutral-display
```

**Why it exists:** The exhibit has two parts:
1. This appliance controller (shows power usage)
2. A stories website (shows sustainability information)

Visitors can switch between them using this button.

**Configurable in code:**
- Button text (default: "Return to Carbon Neutral Stories")
- Button color (default: blue)
- Destination URL

---

### Auto-Return to Stories (Inactivity Timeout)

If no one touches the buttons for a set time, the page automatically redirects:

```
┌─────────────────────────────────────────────────────────────────────┐
│                    INACTIVITY REDIRECT                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   Visitor opens appliance controller                               │
│              │                                                      │
│              ▼                                                      │
│   Timer starts (default: 5 minutes)                                │
│              │                                                      │
│   ┌──────────┴──────────┐                                          │
│   │                     │                                          │
│   ▼                     ▼                                          │
│   Button pressed?       No interaction                             │
│   │                     │                                          │
│   ▼                     ▼                                          │
│   Timer RESETS          Timer counts down                          │
│   (starts over)         │                                          │
│                         ▼                                          │
│                    Timer reaches 0                                 │
│                         │                                          │
│                         ▼                                          │
│                    AUTO-REDIRECT to Stories website                │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

**Configurable in code:** The timeout duration (default: 5 minutes / 300 seconds)

---

### URL Parameters

You can control behavior by adding parameters to the URL:

| Parameter | What It Does | Example |
|-----------|--------------|---------|
| `timeout` | Seconds before auto-redirect | `?timeout=300` |
| `redirectURL` | Where to redirect to | `?redirectURL=https://example.com` |

**Full Example:**
```
http://192.168.1.100/?timeout=300&redirectURL=https://oberlin.communityhub.cloud/digital-signage-v2-fe/remote/oc-carbon-neutral-display
```

This makes the controller page redirect to the stories page after 5 minutes of no interaction.

---

## All Configurable Parameters

Everything that can be changed in the code:

### Relay Durations (onduration)

How long each relay stays ON:

```cpp
unsigned long onDurations[] = {
  10000,   // [0] onduration1 - Relay 1 - 10 seconds
  10000,   // [1] onduration2 - Relay 2 - 10 seconds
  10000,   // [2] onduration3 - Relay 3 - 10 seconds
  63000,   // [3] onduration4 - Relay 4 - 63 seconds
  7000,    // [4] onduration5 - Relay 5 - 7 seconds
  30000,   // [5] onduration6 - Relay 6 - 30 seconds
  70000,   // [6] onduration7 - Relay 7 - 70 seconds
  86000    // [7] onduration8 - Relay 8 - 86 seconds
};
```

**Convert time to milliseconds:** `seconds × 1000`

| Time | Value |
|------|-------|
| 10 seconds | 10000 |
| 1 minute | 60000 |
| 5 minutes | 300000 |

---

### Button Text Labels

What text appears on each button:

```cpp
// text1, text2, text3, etc.
"1. Light-Incandescent"
"2. Light - Compact Fluorescent"
"3. Light - LED"
"4. Lava Lamp"
"5. Hair Dryer"
"6. Mini-Fridge"
"7. Meters & Data"
"8. Geothermal"
```

To change: Find the button HTML in the code and edit the text between `>` and `</button>`.

---

### Night Mode Settings

When to turn everything off:

```cpp
const int lavaOnlyStartHour = 0;   // OFF starts at midnight (0:00)
const int lavaOnlyEndHour = 6;     // OFF ends at 6:00 AM
```

- Set both to `0` to disable night mode
- Uses 24-hour format (0-23)

---

### WiFi Credentials

```cpp
#define STASSID "YourNetworkName"
#define STAPSK "YourPassword"
```

---

### Return Button Settings

```cpp
// Button text
"Return to Carbon Neutral Stories"

// Destination URL
"https://oberlin.communityhub.cloud/digital-signage-v2-fe/remote/oc-carbon-neutral-display"
```

---

### Inactivity Timeout

How long before auto-redirect (in the URL or code):

```cpp
// Default: 300 seconds (5 minutes)
timeout=300
```

---

### Timezone

For accurate night mode scheduling:

```cpp
#define TIMEZONE_STRING "EST5EDT,M3.2.0/2,M11.1.0/2"
```

| Location | String |
|----------|--------|
| Eastern | `"EST5EDT,M3.2.0/2,M11.1.0/2"` |
| Central | `"CST6CDT,M3.2.0/2,M11.1.0/2"` |
| Mountain | `"MST7MDT,M3.2.0/2,M11.1.0/2"` |
| Pacific | `"PST8PDT,M3.2.0/2,M11.1.0/2"` |

---

### Access Point Fallback

If WiFi fails, ESP32 creates its own network:

```cpp
const char* apPassword = "exhibit123";
// Network name: ExhibitControls-XXXX (auto-generated)
```

---

### GPIO Pin Assignments

Which ESP32 pins connect to which relays:

```cpp
const int relayPins[] = {4, 5, 18, 19, 21, 22, 23, 25};
//                       │  │   │   │   │   │   │   │
//                       │  │   │   │   │   │   │   └── Relay 8
//                       │  │   │   │   │   │   └────── Relay 7
//                       │  │   │   │   │   └────────── Relay 6
//                       │  │   │   │   └────────────── Relay 5
//                       │  │   │   └────────────────── Relay 4
//                       │  │   └────────────────────── Relay 3
//                       │  └────────────────────────── Relay 2
//                       └────────────────────────────── Relay 1
```

**Avoid:** GPIO 0, 2, 12 (boot pins) and 34-39 (input only)

---

## Hardware Setup

### Components

| Item | Specification |
|------|---------------|
| Microcontroller | ESP32 Development Board |
| Relay Module | SainSmart 8-Channel 5V Relay Module (Active LOW) |
| Power | 5V DC power supply for ESP32 and relays |
| AC Power | 120V for the appliances (through relay contacts) |

### Wiring Diagram

```
                         ┌─────────────────┐
    5V DC Power ────────►│ VIN         5V ├────────► Relay Module VCC
                         │                 │
    Ground ─────────────►│ GND        GND ├────────► Relay Module GND
                         │                 │
                         │      ESP32      │
                         │                 │
    Relay 1 (IN1) ◄──────┤ GPIO 4          │
    Relay 2 (IN2) ◄──────┤ GPIO 5          │
    Relay 3 (IN3) ◄──────┤ GPIO 18         │
    Relay 4 (IN4) ◄──────┤ GPIO 19         │
    Relay 5 (IN5) ◄──────┤ GPIO 21         │
    Relay 6 (IN6) ◄──────┤ GPIO 22         │
    Relay 7 (IN7) ◄──────┤ GPIO 23         │
    Relay 8 (IN8) ◄──────┤ GPIO 25         │
                         └─────────────────┘
```

### Power Notes

- ESP32 receives 5V DC through VIN and GND pins
- Relay module receives 5V from ESP32's 5V pin
- Program auto-boots when power is applied (no button press needed)
- If power is lost and restored, the system restarts automatically

---

## Software Installation

### Step 1: Install Arduino IDE

1. Download from: https://www.arduino.cc/en/software
2. Install on your computer

### Step 2: Add ESP32 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences**
3. In "Additional Board Manager URLs" add:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
4. Click OK
5. Go to **Tools → Board → Boards Manager**
6. Search for "ESP32"
7. Install "esp32 by Espressif Systems"

### Step 3: Select Board Settings

| Setting | Value |
|---------|-------|
| Board | ESP32 Dev Module |
| Upload Speed | 921600 |
| Flash Frequency | 80MHz |
| Partition Scheme | Default 4MB with spiffs |

### Step 4: Upload Code

1. Connect ESP32 to computer via USB
2. Select port: **Tools → Port → [your ESP32 port]**
3. Open `ESP32_RelayController.ino`
4. Click Upload (→ button)
5. **If upload fails:** Hold BOOT button on ESP32 while uploading

### Step 5: Find the IP Address

1. Open **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. Press RESET on ESP32
4. Look for a line showing the IP address, like:
   ```
   Connected! IP address: 192.168.1.100
   ```
5. Open that address in a web browser

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Code won't upload | Hold BOOT button during "Connecting..." message |
| WiFi won't connect | Check SSID/password, must be 2.4GHz network |
| Can't find IP address | Check Serial Monitor at 115200 baud |
| Web page won't load | Make sure you're on same WiFi as ESP32 |
| Relays not clicking | Check wiring, verify relay module is Active LOW |
| Wrong relay turns on | Check GPIO pin assignments match your wiring |
| Night mode not working | Verify timezone setting is correct |
| Redirect not working | Need both `timeout` AND `redirectURL` parameters |
| Buttons not responding | Check if in night mode (buttons disabled) |

---

## API Endpoints

For advanced users / integration:

| Endpoint | Method | What It Does |
|----------|--------|--------------|
| `/` | GET | Main web interface |
| `/relay?num=X` | GET | Turn on relay X (1-8) |
| `/relayStates` | GET | Get JSON array of all relay states |

---

## License

Open source for educational and non-commercial use.

---

*Part of the Carbon Neutral Exhibition at Oberlin College*
