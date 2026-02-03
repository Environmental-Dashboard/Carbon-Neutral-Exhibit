# Carbon Neutral Exhibition

An interactive museum exhibition demonstrating sustainability, energy efficiency, and environmental monitoring concepts. This repository contains all the components that make up the Carbon Neutral Exhibition display system.

---

## Overview

The Carbon Neutral Exhibition is an educational display that helps visitors understand:

- **Water Usage**: How water flows through meters and how consumption is measured
- **Electricity Consumption**: How electrical current flows and how usage is tracked
- **Data Transmission**: How meter readings are sent to data loggers for monitoring
- **Energy Efficiency**: Comparing different technologies and their environmental impact
- **Geothermal Energy**: How heat is transferred from underground for heating/cooling

---

## Exhibition Components

This repository contains multiple sub-projects, each controlling a different part of the exhibition:

| Folder | Description | Status |
|--------|-------------|--------|
| [Water-Electricity-Metering-System](./Water-Electricity-Metering-System/) | LED visualization showing water and electricity flow with animated LED strips | ✅ Complete |
| [ESP32_RelayController](./ESP32_RelayController/) | Web-based relay controller for 8 exhibit devices with auto-cycle and night mode | ✅ Complete |
| [Geothermal_System](./Geothermal_System/) | LED visualization showing geothermal heat flow with color gradient animation | ✅ Complete |

---

## Project Structure

```
CARBON-NEUTRAL-EXHIBITION/
│
├── README.md                              ← You are here
│
├── Water-Electricity-Metering-System/     ← LED flow visualization
│   ├── Water-Electricity-Metering-System.ino
│   └── README.md
│
├── ESP32_RelayController/                 ← 8-channel relay controller
│   ├── ESP32_RelayController.ino
│   ├── ElectricityButton.gif
│   └── README.md
│
├── Geothermal_System/                     ← Geothermal heat flow visualization
│   ├── Geothermal_system.ino
│   └── README.md
│
└── [Future Components]/                   ← More exhibits to be added
```

---

## Hardware Overview

### Water-Electricity-Metering-System

Controls LED strips that animate in sequence to show:

| LED Strip | Purpose | Color |
|-----------|---------|-------|
| Water Flow | Physical water movement through meters | 🔵 Sea Blue |
| Water Data | Data transmission from water meters | 🟠 Orange |
| Electricity Flow | Electric current in main line | 🟠 Orange |
| Electricity Data | Electrical measurements to logger | 🟠 Orange |

**Hardware Required:**
- ESP32 microcontroller
- WS2812B LED strips
- 5V power supply

👉 See [Water-Electricity-Metering-System/README.md](./Water-Electricity-Metering-System/README.md) for detailed setup instructions.

---

### ESP32_RelayController

Controls 8 relay channels for exhibit devices with a web interface:

| Relay | Device | Duration |
|-------|--------|----------|
| 1 | Light - Incandescent | 10 sec |
| 2 | Light - CFL | 10 sec |
| 3 | Light - LED | 10 sec |
| 4 | Lava Lamp | 63 sec |
| 5 | Hair Dryer | 7 sec |
| 6 | Mini-Fridge | 30 sec |
| 7 | Meters & Data | 70 sec |
| 8 | Geothermal | 86 sec |

**Features:**
- Mobile-friendly web interface
- Auto-cycle mode for unattended operation
- Night mode (Lava Lamp only during off-hours)
- Dual WiFi mode (connects to network or creates access point)

**Hardware Required:**
- ESP32 microcontroller
- 8-channel relay module
- 5V power supply

👉 See [ESP32_RelayController/README.md](./ESP32_RelayController/README.md) for detailed setup instructions.

---

### Geothermal_System

Visualizes how a geothermal heat pump works with automatic summer/winter mode switching:

| Mode | Animation | What It Shows |
|------|-----------|---------------|
| ☀️ Summer | RED → BLUE | Heat extracted from building, dumped to ground |
| ❄️ Winter | BLUE → RED | Heat extracted from ground, pumped to building |

**Features:**
- Automatic alternation between summer and winter modes (4 cycles each)
- Single "globule" animation with distance-based color changes
- Indicator LED outputs for summer/winter mode display
- Fully documented code for easy customization
- 10 configurable color segments with adjustable distances

**Hardware Required:**
- ESP32 microcontroller
- WS2812B LED strip (44 LEDs)
- 5V power supply
- 2x indicator LEDs (GPIO 4 & 5)

👉 See [Geothermal_System/README.md](./Geothermal_System/README.md) for detailed setup instructions.

---

## Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) installed
- ESP32 board support installed in Arduino IDE
- FastLED library installed

### Quick Start

1. Clone this repository:
   ```bash
   git clone https://github.com/Environmental-Dashboard/Carbon-Neutral-Exhibit.git
   ```

2. Navigate to the component you want to set up:
   ```bash
   cd Carbon-Neutral-Exhibit/Geothermal_System
   ```

3. Open the `.ino` file in Arduino IDE

4. Follow the README in that folder for wiring and configuration

---

## Adding New Components

To add a new exhibit component to this repository:

1. Create a new folder with a descriptive name
2. Add your code files
3. Include a README.md with setup instructions
4. Update this main README to list the new component

---

## Contributing

Contributions are welcome! To contribute:

1. Fork this repository
2. Create a feature branch
3. Add your changes
4. Submit a pull request

---

## License

This project is open source and available for educational and exhibition purposes.

---

## Contact

Part of the [Environmental Dashboard](https://github.com/Environmental-Dashboard) project.

---

*Carbon Neutral Exhibition - Inspiring sustainable thinking through interactive displays*

## Credit
Frank Kusi Appiah


