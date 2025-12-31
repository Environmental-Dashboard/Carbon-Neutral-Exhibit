# ESP32 OTA (Over-The-Air) Update Setup

Upload code to your ESP32s wirelessly via WiFi - no USB cable needed after initial setup!

---

## What Is OTA?

**OTA = Over-The-Air Updates**

Instead of connecting a USB cable every time you want to update code, OTA lets you upload new sketches through your WiFi network.

---

## How It Works

```
┌─────────────────────────────────────────────────────────────────────┐
│                        OTA UPDATE FLOW                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│   FIRST TIME (USB Required):                                       │
│   ┌──────────┐    USB     ┌──────────┐                             │
│   │ Computer │ ─────────► │  ESP32   │                             │
│   └──────────┘            └──────────┘                             │
│        │                       │                                    │
│        │  Upload OTA sketch    │                                    │
│        └───────────────────────┘                                    │
│                                                                     │
│   EVERY TIME AFTER (WiFi Only):                                    │
│   ┌──────────┐   WiFi    ┌──────────┐                              │
│   │ Computer │ ~~~~~~~~► │  ESP32   │                              │
│   └──────────┘           └──────────┘                              │
│        │                       │                                    │
│        │  Upload ANY sketch    │                                    │
│        │  wirelessly!          │                                    │
│        └───────────────────────┘                                    │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Setup Instructions

### Step 1: Configure WiFi Credentials

Open `ESP32_OTA_Setup.ino` and change these lines:

```cpp
const char* ssid = "ObieConnect";           // Your WiFi name
const char* password = "122ElmStreet";      // Your WiFi password
```

### Step 2: Upload Via USB (First Time Only)

1. Connect ESP32 to computer with USB cable
2. Open `ESP32_OTA_Setup.ino` in Arduino IDE
3. Select: **Tools → Board → ESP32 Dev Module**
4. Select: **Tools → Port → [Your USB port]**
5. Click **Upload**

### Step 3: Find the ESP32's Network Address

1. Open **Tools → Serial Monitor** (set to 115200 baud)
2. Press RESET on the ESP32
3. Look for output like:
   ```
   ✓ WiFi Connected!
   IP Address: 192.168.1.105
   
   OTA UPDATE READY!
   Hostname: esp32-exhibit
   ```

### Step 4: Upload Wirelessly

1. In Arduino IDE, go to **Tools → Port**
2. Look for a new entry like:
   ```
   esp32-exhibit at 192.168.1.105
   ```
3. Select it
4. Open any sketch (like the Relay Controller)
5. Click **Upload** - it goes over WiFi!

---

## Uploading Other Sketches with OTA

**Important:** Your other sketches (Relay Controller, LED Visualization, etc.) need OTA code added to work with wireless updates.

### Add OTA to Any Sketch

Add these includes at the top:
```cpp
#include <WiFi.h>
#include <ArduinoOTA.h>
```

Add to `setup()`:
```cpp
// After WiFi connects...
ArduinoOTA.setHostname("esp32-relay");  // Change name for each device
ArduinoOTA.begin();
```

Add to `loop()`:
```cpp
ArduinoOTA.handle();  // Must be in loop!
```

---

## Multiple ESP32s

Give each ESP32 a unique hostname to identify them:

| Device | Hostname | Code |
|--------|----------|------|
| Relay Controller | `esp32-relay` | `ArduinoOTA.setHostname("esp32-relay");` |
| LED Visualization | `esp32-led` | `ArduinoOTA.setHostname("esp32-led");` |
| Backup/Test | `esp32-test` | `ArduinoOTA.setHostname("esp32-test");` |

In Arduino IDE port menu, you'll see:
```
esp32-relay at 192.168.1.100
esp32-led at 192.168.1.101
esp32-test at 192.168.1.102
```

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| ESP32 not appearing in port menu | Make sure computer is on same WiFi network |
| Upload fails with timeout | Check ESP32 is powered and connected to WiFi |
| "No response from device" | Reset ESP32 and try again |
| Wrong sketch uploaded | Make sure you selected the right port |
| Can't find IP address | Check Serial Monitor at 115200 baud |

---

## Configuration Options

### Set OTA Password (Optional)

For security, you can require a password for OTA uploads:

```cpp
ArduinoOTA.setPassword("your-password");
```

When uploading, Arduino IDE will prompt for the password.

### Change Hostname

```cpp
const char* otaHostname = "esp32-exhibit";  // Change this
```

---

## Notes

- ESP32 must be powered on and connected to WiFi to receive OTA updates
- Both computer and ESP32 must be on the **same WiFi network**
- OTA upload may be slightly slower than USB, but much more convenient
- If OTA fails, you can always go back to USB upload

---

*Part of the Carbon Neutral Exhibition*

