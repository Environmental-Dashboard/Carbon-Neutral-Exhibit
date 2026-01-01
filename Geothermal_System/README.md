# Geothermal System - LED Visualization

Part of the **Carbon Neutral Exhibition** project by [Environmental Dashboard](https://github.com/Environmental-Dashboard).

This component visualizes how a **geothermal heat pump** works in both summer (cooling) and winter (heating) modes using an animated LED strip.

---

## 🎯 What It Shows

A single "globule" travels along the LED strip, changing color to represent **heat transfer**:

### Summer Mode (Cooling)
Heat is extracted FROM the building and dumped INTO the ground.

```
🔴 RED → 🟠 ORANGE → ⚪ WHITE → 🔵 LIGHT BLUE → 🔵 BLUE
(Hot)    (Warm)      (Exchanger) (Cool)         (Cold)
```

### Winter Mode (Heating)
Heat is extracted FROM the ground and pumped INTO the building.

```
🔵 BLUE → 🔵 LIGHT BLUE → ⚪ WHITE → 🟠 ORANGE → 🔴 RED
(Cold)    (Cool)          (Exchanger) (Warm)     (Hot)
```

The system automatically alternates between **4 summer cycles** and **4 winter cycles**, with indicator LEDs showing the current mode.

---

## 🔧 Hardware Requirements

| Component | Specification | Notes |
|-----------|---------------|-------|
| Microcontroller | ESP32 Dev Module | Any ESP32 variant works |
| LED Strip | WS2812B (44 LEDs) | 24V addressable RGB LEDs |
| Power Supply | 24V DC | Main power for LED strip |
| Step-Down Converter | 24V → 5V DC-DC | Powers ESP32 and relay module |
| Relay Module | 2-channel, 5V | Controls summer/winter signs |
| Summer Sign | Via relay on GPIO 4 | Illuminated sign for summer mode |
| Winter Sign | Via relay on GPIO 5 | Illuminated sign for winter mode |

### Wiring Diagram

```
                         24V POWER SUPPLY
                         ┌─────────────────┐
                         │  24V (+)  ──────┼───────────────────┐
                         │  GND (-)  ──────┼─────────┬─────────┼──── Common GND
                         └─────────────────┘         │         │
                                                     │         │
                         STEP-DOWN CONVERTER         │         │
                         (24V → 5V)                  │         │
                         ┌─────────────────┐         │         │
              24V ───────│ IN+      OUT+ ──┼─────────┼─────────┼──── 5V Rail
              GND ───────│ IN-      OUT- ──┼─────────┤         │
                         └─────────────────┘         │         │
                                                     │         │
                                                     │         │
   ESP32                                             │         │
   ┌───────────────────┐                             │         │
   │  5V  ─────────────┼─────────────────────────────┤         │
   │  GND ─────────────┼─────────────────────────────┴─────────┤
   │  GPIO 2 (D2) ─────┼───────────────────────────────────────┼──── LED Strip DIN
   │  GPIO 4 (D4) ─────┼──── Relay IN1 (Summer)                │
   │  GPIO 5 (D5) ─────┼──── Relay IN2 (Winter)                │
   └───────────────────┘                                       │
                                                               │
                                                               │
   LED STRIP (24V)                                             │
   ┌───────────────────┐                                       │
   │  24V ─────────────┼───────────────────────────────────────┘
   │  GND ─────────────┼──── Common GND
   │  DIN ─────────────┼──── GPIO 2
   └───────────────────┘


   2-CHANNEL RELAY MODULE
   ┌──────────────────────────────────────────────────────────────┐
   │                                                              │
   │  VCC ──────────── 5V (from step-down converter)              │
   │  GND ──────────── Common GND                                 │
   │  IN1 ──────────── GPIO 4 (Summer control signal)             │
   │  IN2 ──────────── GPIO 5 (Winter control signal)             │
   │                                                              │
   │  Relay 1 (Summer Sign):                                      │
   │    COM1 ─────────── Sign Power Source (+)                    │
   │    NO1  ─────────── Summer Sign (+)                          │
   │    (Sign GND connects to power source GND)                   │
   │                                                              │
   │  Relay 2 (Winter Sign):                                      │
   │    COM2 ─────────── Sign Power Source (+)                    │
   │    NO2  ─────────── Winter Sign (+)                          │
   │    (Sign GND connects to power source GND)                   │
   │                                                              │
   └──────────────────────────────────────────────────────────────┘
```

### Power Flow Summary

```
24V Supply ──┬──► LED Strip (24V input)
             │
             └──► Step-Down Converter (24V → 5V) ──┬──► ESP32 (5V)
                                                   │
                                                   └──► Relay Module VCC (5V)

All GND connections are shared (common ground)
```

⚠️ **Important Notes:**
- The LED strip runs on **24V** - do NOT connect 5V to it
- ESP32 and relay module run on **5V** from the step-down converter
- All ground connections must be tied together (common ground)
- The relay module switches the signs ON/OFF based on ESP32 GPIO signals

---

## 📦 Software Setup

### 1. Install Arduino IDE
Download from [arduino.cc](https://www.arduino.cc/en/software)

### 2. Add ESP32 Board Support
- Go to: **File → Preferences**
- Add to "Additional Board URLs":
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- Go to: **Tools → Board → Boards Manager**
- Search "ESP32" and click **Install**

### 3. Install FastLED Library
- Go to: **Sketch → Include Library → Manage Libraries**
- Search "FastLED" and click **Install**

### 4. Configure Board Settings
| Setting | Value |
|---------|-------|
| Board | ESP32 Dev Module |
| Upload Speed | 115200 |
| Port | (your ESP32's COM port) |

---

## ⚙️ Configuration Guide

### Quick Reference Table

| Setting | Variable | Default | Description |
|---------|----------|---------|-------------|
| LED Data Pin | `LED_PIN` | 2 | GPIO for LED strip data |
| Number of LEDs | `NUM_LEDS` | 44 | Total LEDs in strip |
| Brightness | `BRIGHTNESS` | 150 | Master brightness (0-255) |
| Animation Speed | `speedMs` | 200 | Delay per step (ms) |
| Summer Indicator | `SUMMER_PIN` | 4 | GPIO for summer LED |
| Winter Indicator | `WINTER_PIN` | 5 | GPIO for winter LED |
| Color Order | `COLOR_ORDER` | RGB | Try GRB if colors wrong |

---

### 📏 Changing Segment Distances

The globule changes color after traveling a set number of LEDs. Adjust these values to match your physical display:

```cpp
// Outbound journey (phases 0-4)
#define d1   5    // RED zone
#define d2   5    // ORANGE zone
#define d3   7    // WHITE zone
#define d4   5    // LIGHT BLUE zone
#define d5   5    // BLUE zone

// Return journey (phases 5-9)
#define d6   2    // BLUE zone
#define d7   3    // LIGHT BLUE zone
#define d8   2    // WHITE zone
#define d9   3    // ORANGE zone
#define d10  2    // RED zone
```

**Total LEDs used:** d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + d10 = 39

---

### 🎨 Changing Colors

Colors are defined using RGB values (0-255 for each channel):

```cpp
#define S_RED        CRGB::Red              // (255, 0, 0)
#define S_ORANGE     CRGB(255, 120, 0)      // Warm orange
#define S_WHITE      CRGB(255, 220, 200)    // Warm white
#define S_LIGHTBLUE  CRGB(0, 80, 255)       // Cool blue
#define S_BLUE       CRGB::Blue             // (0, 0, 255)
```

**Color Mixing Guide:**

| Desired Color | RGB Value |
|---------------|-----------|
| Pure Red | `CRGB(255, 0, 0)` |
| Orange | `CRGB(255, 120, 0)` |
| Yellow | `CRGB(255, 255, 0)` |
| Warm White | `CRGB(255, 220, 200)` |
| Cool White | `CRGB(200, 220, 255)` |
| Light Blue | `CRGB(0, 80, 255)` |
| Pure Blue | `CRGB(0, 0, 255)` |

---

### ⏱️ Changing Animation Speed

```cpp
int speedMs = 200;  // milliseconds per LED step
```

| Value | Speed | Description |
|-------|-------|-------------|
| 50-100 | Fast | Energetic, attention-grabbing |
| 150-250 | Medium | Easy to follow (default) |
| 300-500 | Slow | Dramatic, meditative |

---

### 🔄 Changing Number of Cycles

By default, the system runs **4 cycles** in each mode before switching. To change:

```cpp
// In loop() function:
for (int i = 0; i < 4; i++) {   // ← Change 4 to desired number
    runGlobuleCycle(false);
}
```

---

## 🚀 Uploading the Code

1. Connect ESP32 via USB cable (must be data cable, not power-only)
2. Select correct COM port in **Tools → Port**
3. Click **Upload** button
4. If upload fails:
   - Hold the **BOOT** button on ESP32 while uploading
   - Release when upload starts
   - Try lowering upload speed to 115200

---

## 🔍 Troubleshooting

| Problem | Solution |
|---------|----------|
| Colors look wrong | Change `COLOR_ORDER` to `GRB` or `BGR` |
| LEDs flicker | Add 1000µF capacitor across 5V and GND |
| First LED wrong color | Add 300-500Ω resistor on data line |
| No LEDs light up | Check wiring, power supply, pin number |
| Upload fails | Hold BOOT button, lower upload speed |
| Too bright/dim | Adjust `BRIGHTNESS` (0-255) |
| Animation too fast/slow | Adjust `speedMs` |

---

## 📁 Files

```
Geothermal_System/
├── Geothermal_system.ino   ← Main code (fully documented)
└── README.md               ← This file
```

---

## 🌡️ How Geothermal Heat Pumps Work

This display demonstrates the real-world process:

**Summer (Cooling Mode):**
1. Indoor unit absorbs heat from building air
2. Heat pump compresses refrigerant (adds more heat)
3. Heat is transferred to ground loop fluid
4. Fluid carries heat underground where it's absorbed by earth
5. Cooled fluid returns to repeat cycle

**Winter (Heating Mode):**
1. Ground loop extracts heat from underground (earth stays ~55°F year-round)
2. Heat pump concentrates this low-grade heat
3. Heat is released into building air
4. Cooled fluid returns underground to collect more heat

The LED animation shows this heat transfer process with colors representing temperature!

---

## 🔮 Future Enhancements

- [ ] Add temperature sensor to affect animation based on real temperature
- [ ] Add web interface for remote control
- [ ] Add button to manually switch between modes
- [ ] Add display showing energy savings calculation

---

## 📄 License

Open source for educational and exhibition purposes.
Part of the [Carbon Neutral Exhibition](https://github.com/Environmental-Dashboard/Carbon-Neutral-Exhibit) project.

---

*Last updated: January 2026*
