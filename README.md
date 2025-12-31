# 🌊⚡ Carbon Neutral Exhibition - LED Visualization System

A visual display that shows water and electricity flow using LED strips. Perfect for exhibitions demonstrating sustainability and energy monitoring concepts.

---

## 📋 Table of Contents

1. [What This Does](#what-this-does)
2. [Hardware Requirements](#hardware-requirements)
3. [Wiring Guide](#wiring-guide)
4. [Software Setup](#software-setup)
5. [Customization Guide](#customization-guide)
6. [Troubleshooting](#troubleshooting)
7. [Quick Reference](#quick-reference)

---

## 🎯 What This Does

This system controls **4 LED strips** that animate in sequence, like actors on a stage:

| Order | LED Strip | What It Represents | Color | Direction |
|-------|-----------|-------------------|-------|-----------|
| 1st | Water Flow | Physical water moving through meters | 🔵 Sea Blue | Forward → |
| 2nd | Water Data | Data transmitted from water meters | 🟠 Orange | Backward ← |
| 3rd | Electricity Flow | Electric current in the main line | 🟠 Orange | Forward → |
| 4th | Electricity Data | Electrical measurements sent to logger | 🟠 Orange | Backward ← |

**Animation Pattern:**
1. A glowing dot travels across Strip 1 (4 times)
2. Strip 1 turns off
3. A glowing dot travels across Strip 2 (4 times)
4. Strip 2 turns off
5. ... and so on for Strips 3 and 4
6. All strips go dark for 3 seconds
7. The cycle repeats forever

---

## 🔧 Hardware Requirements

### Components Needed

| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32 Development Board | 1 | Any ESP32 variant works |
| WS2812B LED Strip | 4 pieces | Cut to required lengths |
| 5V Power Supply | 1 | At least 2A recommended |
| Jumper Wires | Several | For connections |
| USB Cable | 1 | To program the ESP32 |

### LED Strip Specifications

| Strip Name | Length | Color Order | Notes |
|------------|--------|-------------|-------|
| Water Flow | 20 LEDs | BGR | Check your strip's order |
| Water Data | 5 LEDs | RGB | Check your strip's order |
| Electricity Flow | 15 LEDs | GRB | Check your strip's order |
| Electricity Data | 7 LEDs | RGB | Check your strip's order |

> 💡 **What is "Color Order"?** LED strips receive color data in different sequences. If your LEDs show the wrong colors (e.g., blue instead of red), you may need to change the color order in the code.

---

## 🔌 Wiring Guide

### Pin Connections

```
ESP32 Pin    →    LED Strip
─────────────────────────────
GPIO 2 (D2)  →    Water Flow LEDs (Data In)
GPIO 4 (D4)  →    Water Data LEDs (Data In)
GPIO 18 (D18) →   Electricity Flow LEDs (Data In)
GPIO 5 (D5)  →    Electricity Data LEDs (Data In)
GND          →    All LED strips (GND)
5V/VIN       →    All LED strips (5V)
```

### Wiring Diagram

```
                    ┌─────────────────┐
                    │      ESP32      │
                    │                 │
    Water Flow ←────┤ D2          5V ├────→ LED 5V (all strips)
    Water Data ←────┤ D4         GND ├────→ LED GND (all strips)
    Elec Flow  ←────┤ D18            │
    Elec Data  ←────┤ D5             │
                    └─────────────────┘
```

### Important Wiring Notes

1. **Power**: LED strips can draw significant current. For more than 30 LEDs total, use an external 5V power supply instead of USB power.

2. **Common Ground**: ALL LED strip GND wires must connect to the ESP32's GND.

3. **Data Direction**: Connect to the "DIN" (Data In) end of each LED strip, not "DOUT".

---

## 💻 Software Setup

### Step 1: Install Arduino IDE

1. Download Arduino IDE from: https://www.arduino.cc/en/software
2. Install and open the application

### Step 2: Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences**
3. In "Additional Board Manager URLs", add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search for "ESP32" and install "esp32 by Espressif Systems"

### Step 3: Install FastLED Library

1. Go to **Sketch → Include Library → Manage Libraries**
2. Search for "FastLED"
3. Install "FastLED by Daniel Garcia"

### Step 4: Upload the Code

1. Open `led_visualization.ino` in Arduino IDE
2. Connect your ESP32 via USB
3. Select your board: **Tools → Board → ESP32 Dev Module**
4. Select the correct port: **Tools → Port → (your ESP32's port)**
5. Click the **Upload** button (→ arrow icon)
6. Wait for "Done uploading" message

---

## 🎨 Customization Guide

### Changing Colors

Find the **SECTION 3: COLORS** in the code. Colors use the format `CRGB(Red, Green, Blue)` where each value is 0-255.

```cpp
// Examples:
#define COLOR_SEA_BLUE   CRGB(0, 160, 200)    // Current blue
#define COLOR_ORANGE     CRGB(255, 90, 0)     // Current orange

// Try these alternatives:
// CRGB(255, 0, 0)       = Pure Red
// CRGB(0, 255, 0)       = Pure Green
// CRGB(0, 0, 255)       = Pure Blue
// CRGB(255, 255, 0)     = Yellow
// CRGB(255, 0, 255)     = Purple
// CRGB(0, 255, 255)     = Cyan
// CRGB(255, 255, 255)   = White
```

**Making Orange Less Red:**
```cpp
// Original (reddish):
#define COLOR_ORANGE     CRGB(255, 90, 0)

// Less red (more true orange):
#define COLOR_ORANGE     CRGB(255, 120, 0)

// Even less red:
#define COLOR_ORANGE     CRGB(255, 150, 0)
```

---

### Changing Speed

Find **SECTION 5: ANIMATION SPEED** in the code.

```cpp
// Slower animation (higher number = slower):
#define SPEED_MS_GLOBAL   500    // Half second per LED

// Faster animation (lower number = faster):
#define SPEED_MS_GLOBAL   200    // Fifth of a second per LED

// Current setting:
#define SPEED_MS_GLOBAL   350    // About third of a second
```

**Individual Strip Speeds:**
```cpp
// To make each strip move at different speeds:
#define DELAY_WATER_FLOW   300    // Water flow: faster
#define DELAY_WATER_DATA   400    // Water data: slower
#define DELAY_ELEC_FLOW    300    // Elec flow: faster
#define DELAY_ELEC_DATA    400    // Elec data: slower
```

---

### Changing Brightness

Find **SECTION 4: BRIGHTNESS** in the code.

```cpp
#define BRIGHTNESS_LEVEL   150    // Current setting (0-255)

// Dim for dark rooms:
#define BRIGHTNESS_LEVEL   50

// Very bright:
#define BRIGHTNESS_LEVEL   200

// Maximum (may be too harsh):
#define BRIGHTNESS_LEVEL   255
```

---

### Changing LED Strip Lengths

Find **SECTION 2: LED STRIP LENGTHS** in the code.

```cpp
// If your Water Flow strip has 30 LEDs instead of 20:
#define NUM_WATER_FLOW_LEDS   30

// Count the LEDs on each physical strip and update these numbers:
#define NUM_WATER_FLOW_LEDS   20    // Change to match your strip
#define NUM_WATER_DATA_LEDS   5     // Change to match your strip
#define NUM_ELEC_FLOW_LEDS    15    // Change to match your strip
#define NUM_ELEC_DATA_LEDS    7     // Change to match your strip
```

---

### Changing Animation Behavior

Find **SECTION 6: ANIMATION BEHAVIOR** in the code.

```cpp
// How many times the dot travels across each strip:
#define CYCLES_PER_STAGE        4    // Default: 4 passes
// Try 2 for shorter performance, 6 for longer

// Pause between each strip's performance:
#define PAUSE_BETWEEN_STAGES    500  // Default: half second
// Try 1000 for a full second pause

// Pause after all 4 strips finish before restarting:
#define PAUSE_AFTER_FULL_CYCLE  3000 // Default: 3 seconds
// Try 5000 for 5 seconds of darkness
```

---

## 🔍 Troubleshooting

### Problem: LEDs don't light up at all

| Check | Solution |
|-------|----------|
| Power | Ensure 5V and GND are connected |
| Data wire | Make sure it's connected to DIN, not DOUT |
| Pin numbers | Verify pins in code match your wiring |
| Upload | Make sure code uploaded successfully |

### Problem: Colors are wrong (e.g., blue shows as red)

This means the **color order** is incorrect for your LED strip. Find the `setup()` function and try different color orders:

```cpp
// Original (BGR):
FastLED.addLeds<WS2812B, PIN_WATER_FLOW, BGR>(waterFlowLeds, NUM_WATER_FLOW_LEDS);

// Try RGB:
FastLED.addLeds<WS2812B, PIN_WATER_FLOW, RGB>(waterFlowLeds, NUM_WATER_FLOW_LEDS);

// Try GRB:
FastLED.addLeds<WS2812B, PIN_WATER_FLOW, GRB>(waterFlowLeds, NUM_WATER_FLOW_LEDS);
```

Common color orders: `RGB`, `GRB`, `BGR`, `BRG`, `RBG`, `GBR`

### Problem: Only some LEDs work

| Check | Solution |
|-------|----------|
| LED count | Make sure `NUM_xxx_LEDS` matches your actual strip length |
| Power | Long strips may need more power |
| Damaged LED | A broken LED can stop all LEDs after it |

### Problem: Animation is too fast/slow

Adjust `SPEED_MS_GLOBAL` in Section 5:
- Higher number = slower animation
- Lower number = faster animation

### Problem: LEDs are too bright/dim

Adjust `BRIGHTNESS_LEVEL` in Section 4 (0-255).

---

## 📊 Quick Reference

### Speed Settings (milliseconds per LED)

| Value | Description |
|-------|-------------|
| 50 | Very fast (strobe-like) |
| 100 | Fast |
| 200 | Medium-fast |
| 350 | Medium (default) |
| 500 | Slow |
| 750 | Very slow |
| 1000 | Extremely slow (1 second per LED) |

### Brightness Settings

| Value | Description |
|-------|-------------|
| 25 | Very dim |
| 50 | Dim |
| 100 | Medium |
| 150 | Bright (default) |
| 200 | Very bright |
| 255 | Maximum |

### Common Colors

| Color | Code |
|-------|------|
| Red | `CRGB(255, 0, 0)` |
| Green | `CRGB(0, 255, 0)` |
| Blue | `CRGB(0, 0, 255)` |
| Yellow | `CRGB(255, 255, 0)` |
| Cyan | `CRGB(0, 255, 255)` |
| Purple | `CRGB(255, 0, 255)` |
| White | `CRGB(255, 255, 255)` |
| Orange | `CRGB(255, 90, 0)` |
| Sea Blue | `CRGB(0, 160, 200)` |
| Warm White | `CRGB(255, 200, 150)` |

---

## 📝 License

This project is open source and available for educational and exhibition purposes.

---

## 🤝 Support

If you need help:
1. Check the Troubleshooting section above
2. Verify all wiring connections
3. Make sure the FastLED library is installed
4. Try uploading the unmodified code first

---

*Created for the Carbon Neutral Exhibition*
