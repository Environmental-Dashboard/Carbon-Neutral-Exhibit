# Carbon Neutral Exhibition

An interactive museum exhibition demonstrating sustainability, energy efficiency, and environmental monitoring concepts. This repository contains all the components that make up the Carbon Neutral Exhibition display system.

---

## Overview

The Carbon Neutral Exhibition is an educational display that helps visitors understand:

- **Water Usage**: How water flows through meters and how consumption is measured
- **Electricity Consumption**: How electrical current flows and how usage is tracked
- **Data Transmission**: How meter readings are sent to data loggers for monitoring
- **Energy Efficiency**: Comparing different technologies and their environmental impact

---

## Exhibition Components

This repository contains multiple sub-projects, each controlling a different part of the exhibition:

| Folder | Description | Status |
|--------|-------------|--------|
| [Water-Electricity-Metering-System](./Water-Electricity-Metering-System/) | LED visualization showing water and electricity flow with 4 animated LED strips | ✅ Complete |
| *More components coming soon* | Additional exhibit modules will be added here | 🔄 Planned |

---

## Project Structure

```
CARBON-NEUTRAL-EXHIBITION/
│
├── README.md                              ← You are here
│
├── Water-Electricity-Metering-System/     ← LED flow visualization
│   ├── led_visualization.ino              ← Arduino/ESP32 code
│   └── README.md                          ← Setup & customization guide
│
└── [Future Components]/                   ← More exhibits to be added
```

---

## Hardware Overview

### Water-Electricity-Metering-System

Controls 4 LED strips that animate in sequence to show:

| LED Strip | Purpose | Color |
|-----------|---------|-------|
| Water Flow | Physical water movement through meters | 🔵 Sea Blue |
| Water Data | Data transmission from water meters | 🟠 Orange |
| Electricity Flow | Electric current in main line | 🟠 Orange |
| Electricity Data | Electrical measurements to logger | 🟠 Orange |

**Hardware Required:**
- ESP32 microcontroller
- WS2812B LED strips (4 total)
- 5V power supply

👉 See [Water-Electricity-Metering-System/README.md](./Water-Electricity-Metering-System/README.md) for detailed setup instructions.

---

## Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) installed
- ESP32 board support installed in Arduino IDE
- FastLED library installed

### Quick Start

1. Clone this repository:
   ```bash
   git clone https://github.com/Environmental-Dashboard/CARBON-NEUTRAL-EXHIBITION.git
   ```

2. Navigate to the component you want to set up:
   ```bash
   cd CARBON-NEUTRAL-EXHIBITION/Water-Electricity-Metering-System
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
