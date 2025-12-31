# Water and Electricity Metering System

**System Logic and Visualization Documentation**

## 1. Purpose of This Document

This document explains how the system is intended to work at a high level before any code is written.

It exists so that:

* A future developer can understand the system logic quickly
* Changes can be made without reverse engineering intent from code
* The visualization logic and hardware mapping are clear from the start

This is not a final implementation guide. Code will be written later based on the ideas described here.

---

## 2. System Idea Overview

The system models a campus scale water and electricity metering setup by separating three core concepts:

1. **Physical Flow**
   Water flow and electrical current

2. **Measurement**
   How meters sense that flow

3. **Data Movement**
   How measurements travel to a central data logger

---

## 3. Core Components

### Data Logger

The data logger represents the central point where all measurements are collected.

In a real system, this would store, process, or forward data.
In this project, it serves as the logical destination for all meter data.

### Water Meters

There are two water meters:

* Large water meter
* Small water meter

Both meters measure water flow and generate data as water passes through them.

System assumptions:

* Water physically flows through the meters
* Each meter produces a signal representing usage
* That signal is sent to the data logger

### Electricity Meter

Electricity is measured indirectly using induction.

Process overview:

1. Current flows through a main electrical wire
2. This current creates a magnetic field
3. A nearby coil senses the magnetic field
4. A small induced current is generated
5. The induced signal represents electrical usage

This induced signal is what gets sent to the data logger.

---

## 4. Separation of Flow Types

A key design decision is separating physical flow from data flow.

* **Physical flow** shows how water or electrons move
* **Data flow** shows how information moves to the logger

This separation helps keep the visualization intuitive and logically consistent.

---

## 5. LED Visualization Logic

Each LED strip is assigned a specific role. These roles should remain consistent even if animations or hardware change later.

### Water Flow LEDs

* Meaning: Physical movement of water through meters
* Color: Sea blue `CRGB(0, 160, 200)`
* Pin: D2
* Length: 20 LEDs
* Order: BGR

Represents actual water movement.

### Water Data LEDs

* Meaning: Data traveling from water meters to the logger
* Color: Orange `CRGB(255, 90, 0)`
* Pin: D4
* Length: 5 LEDs
* Order: RGB

Represents water usage data transmission.

### Electricity Flow LEDs

* Meaning: Electron flow in the main electrical line
* Color: Orange `CRGB(255, 90, 0)`
* Pin: D18
* Length: 15 LEDs
* Order: GRB

Represents current flow, not logged data.

### Electricity Data LEDs

* Meaning: Electrical measurement data sent to the logger
* Color: Orange `CRGB(255, 90, 0)`
* Pin: D5
* Length: 7 LEDs
* Order: RGB

---

## 6. Animation Sequence Logic

The system runs a repeating loop made of four stages.
Each stage animates a moving **globule**, a short block of lit LEDs, across a specific LED group.

For each stage:

* Run the globule animation for 4 full cycles
* Turn that LED group completely off
* Move to the next stage

### Stage 1: Water Flow LEDs

* Animate a sea blue globule across the Water Flow LEDs
* Run for 4 cycles
* Turn off LEDs

### Stage 2: Water Data LEDs

* Animate an orange globule across the Water Data LEDs
* Run for 4 cycles
* Turn off LEDs

### Stage 3: Electricity Flow LEDs

* Animate an orange globule across the Electricity Flow LEDs
* Run for 4 cycles
* Turn off LEDs

### Stage 4: Electricity Data LEDs

* Animate an orange globule across the Electricity Data LEDs
* Run for 4 cycles
* Turn off LEDs

### End of Loop Pause

After Stage 4:

* Turn all LEDs off
* Wait 3 seconds
* Restart from Stage 1

---

## 7. Intended Use of This Document

This document should be referenced:

* Before writing any code
* When modifying LED behavior
* When adding new meters or sensors
* When refactoring system logic
