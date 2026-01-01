# Water and Electricity Metering System

A visual LED display system that demonstrates water and electricity flow in a museum/exhibit setting. Uses 4 LED strips with sequential animations to represent physical flow and data transmission.

## Features

- **4 Independent LED Strips**: Water Flow, Water Data, Electricity Flow, Electricity Data
- **Sequential Animation**: Each strip performs like actors on a stage—one at a time
- **Customizable Colors**: Sea blue for water, orange for electricity/data
- **Adjustable Speed**: Control animation timing per strip or globally
- **Beginner-Friendly Code**: Clearly marked sections for easy customization

---

## Hardware Requirements

| Component | Specification |
|-----------|---------------|
| Microcontroller | ESP32 Development Board |
| LED Strips | WS2812B Addressable LED Strips (4 total) |
| Power Supply | 5V 2A minimum (more for longer strips) |
| Wiring | Jumper wires for GPIO connections |

### GPIO Pin Mapping

| LED Strip | Purpose | GPIO Pin | LED Count | Color Order |
|-----------|---------|----------|-----------|-------------|
| Water Flow | Physical water movement | GPIO 2 (D2) | 20 LEDs | BGR |
| Water Data | Data from meters to logger | GPIO 4 (D4) | 5 LEDs | RGB |
| Electricity Flow | Current in main line | GPIO 18 (D18) | 15 LEDs | GRB |
| Electricity Data | Measurements to logger | GPIO 5 (D5) | 7 LEDs | RGB |

---

## Configuration Guide

### 1. LED Strip Lengths

If your strips have different LED counts, update these values:

```cpp
#define NUM_WATER_FLOW_LEDS   20    // Water Flow strip
#define NUM_WATER_DATA_LEDS   5     // Water Data strip
#define NUM_ELEC_FLOW_LEDS    15    // Electricity Flow strip
#define NUM_ELEC_DATA_LEDS    7     // Electricity Data strip
```

---

### 2. Colors

Colors use RGB format: `CRGB(Red, Green, Blue)` where each value is 0-255.

```cpp
#define COLOR_SEA_BLUE   CRGB(0, 160, 200)    // Water Flow - sea blue
```

#### Orange Colors (Per-Strip Tuning)

Each LED strip may display orange differently due to hardware variations. These are tuned individually:

```cpp
#define COLOR_ORANGE_WATER_DATA   CRGB(255, 90, 0)    // Water Data - standard orange
#define COLOR_ORANGE_ELEC_FLOW    CRGB(255, 120, 0)   // Elec Flow - less reddish
#define COLOR_ORANGE_ELEC_DATA    CRGB(255, 150, 0)   // Elec Data - more yellow-orange
```

**To adjust orange if it looks too red:** Increase the middle number (green value).

| Green Value | Result |
|-------------|--------|
| 90 | Deep orange (more red) |
| 120 | Standard orange |
| 150 | Yellow-orange |
| 180 | More yellow |

#### Common Color Values

| Color | Code |
|-------|------|
| Red | `CRGB(255, 0, 0)` |
| Green | `CRGB(0, 255, 0)` |
| Blue | `CRGB(0, 0, 255)` |
| Yellow | `CRGB(255, 255, 0)` |
| Purple | `CRGB(255, 0, 255)` |
| Cyan | `CRGB(0, 255, 255)` |
| White | `CRGB(255, 255, 255)` |

---

### 3. Animation Speed

Speed is in milliseconds. Higher = slower animation.

```cpp
#define SPEED_MS_GLOBAL   350    // All strips use this speed
```

#### Speed Reference

| Value | Description |
|-------|-------------|
| 100 | Fast |
| 200 | Medium-fast |
| 350 | Medium (default) |
| 500 | Slow |
| 1000 | Very slow (1 sec/LED) |

#### Individual Strip Speeds (Current Settings)

```cpp
#define DELAY_WATER_FLOW   SPEED_MS_GLOBAL   // Uses global (350ms)
#define DELAY_WATER_DATA   SPEED_MS_GLOBAL   // Uses global (350ms)
#define DELAY_ELEC_FLOW    250               // Faster electricity flow
#define DELAY_ELEC_DATA    SPEED_MS_GLOBAL   // Uses global (350ms)
```

To customize, replace `SPEED_MS_GLOBAL` with a number (in milliseconds).

---

### 4. Brightness

```cpp
#define BRIGHTNESS_LEVEL   150    // Range: 0-255
```

| Value | Description |
|-------|-------------|
| 50 | Dim (dark rooms) |
| 100 | Medium |
| 150 | Bright (default) |
| 255 | Maximum |

---

### 5. Animation Behavior

```cpp
#define CYCLES_PER_STAGE        4       // Passes per strip before next
#define PAUSE_BETWEEN_STAGES    500     // Pause between strips (ms)
#define PAUSE_AFTER_FULL_CYCLE  3000    // Pause after all 4 complete (ms)
```

---

### 6. Color Order Fix

If colors appear wrong (e.g., blue instead of red), change the color order in `setup()`:

```cpp
FastLED.addLeds<WS2812B, PIN_WATER_FLOW, BGR>(waterFlowLeds, NUM_WATER_FLOW_LEDS);
//                                       ^^^
//                       Try: RGB, GRB, BGR, BRG, RBG, GBR
```

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

2. **Install FastLED Library**:
   - Go to `Sketch` → `Include Library` → `Manage Libraries`
   - Search "FastLED" and install "FastLED by Daniel Garcia"

3. **Select Board**:
   - `Tools` → `Board` → `ESP32 Dev Module`

4. **Upload the Code**:
   - Connect ESP32 via USB
   - Select the correct Port in `Tools` → `Port`
   - Click Upload
   - If upload fails, hold BOOT button on ESP32 during "Connecting..."

---

## Wiring Diagram

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

**Important**: 
- Connect to **DIN** (Data In) on LED strips, not DOUT
- All GND wires must connect to ESP32 GND
- For 30+ total LEDs, use external 5V power supply

---

## Animation Sequence

The system runs a continuous loop:

| Act | LED Strip | Color | Direction | Meaning |
|-----|-----------|-------|-----------|---------|
| 1 | Water Flow | Sea Blue | Forward → | Water moving through meters |
| 2 | Water Data | Orange | Backward ← | Data sent to logger |
| 3 | Electricity Flow | Orange | Forward → | Current in main line |
| 4 | Electricity Data | Orange | Backward ← | Measurements to logger |

After Act 4, all LEDs turn off for 3 seconds, then the cycle repeats.

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| LEDs don't light up | Check 5V and GND connections |
| Wrong colors | Change color order (BGR/RGB/GRB) in setup() |
| Only some LEDs work | Verify LED count matches `NUM_xxx_LEDS` value |
| Animation too fast/slow | Adjust `SPEED_MS_GLOBAL` value |
| LEDs too dim/bright | Adjust `BRIGHTNESS_LEVEL` (0-255) |
| Upload fails | Hold BOOT button during "Connecting..." |
| Code won't compile | Ensure FastLED library is installed |

---

## File Structure

```
Water-Electricity-Metering-System/
├── led_visualization/
│   └── led_visualization.ino    # Main Arduino sketch
└── README.md                     # This file
```

---

## License

This project is open source and available for educational and exhibition purposes.

---

## Contributing

Contributions are welcome! Please submit issues or pull requests to improve this project.

---

*Water and Electricity Metering System - LED Visualization*
