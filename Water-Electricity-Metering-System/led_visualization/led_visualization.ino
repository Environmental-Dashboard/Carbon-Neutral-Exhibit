/*
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║              WATER AND ELECTRICITY METERING SYSTEM                        ║
 * ╠═══════════════════════════════════════════════════════════════════════════╣
 * ║  This code controls 4 LED strips that visualize water and electricity     ║
 * ║  flow in an exhibition. Each strip lights up one at a time, like actors   ║
 * ║  performing on a stage - one finishes, then the next begins.              ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 *
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │                        WHAT EACH LED STRIP REPRESENTS                   │
 * ├─────────────────────────────────────────────────────────────────────────┤
 * │  1. WATER FLOW       → Physical water moving through meters (Sea Blue)  │
 * │  2. WATER DATA       → Data from water meters sent to logger (Orange)   │
 * │  3. ELECTRICITY FLOW → Electric current in the main line (Orange)       │
 * │  4. ELECTRICITY DATA → Electrical data sent to logger (Orange)          │
 * └─────────────────────────────────────────────────────────────────────────┘
 *
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │                           HOW TO USE THIS FILE                          │
 * ├─────────────────────────────────────────────────────────────────────────┤
 * │  1. Look for sections marked "CHANGE THIS" - these are safe to modify   │
 * │  2. Numbers control speed, brightness, colors, and timing               │
 * │  3. After changing a number, upload the code to see the effect          │
 * │  4. If something breaks, re-download the original file                  │
 * └─────────────────────────────────────────────────────────────────────────┘
 */

#include <FastLED.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

/* ═══════════════════════════════════════════════════════════════════════════
 *                    SECTION 0: WIFI & OTA CONFIGURATION  ← CHANGE THIS
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 *  WiFi credentials for OTA (Over-The-Air) updates.
 *  After initial USB upload, you can update this code via WiFi!
 *  In Arduino IDE: Tools → Port → select "esp32-led at [IP address]"
 */

const char* wifiSSID = "ObieConnect";        // <-- Your WiFi network name
const char* wifiPassword = "122ElmStreet";   // <-- Your WiFi password
const char* otaHostname = "esp32-led";       // <-- Name shown in Arduino IDE
const char* otaPassword = "132ElmStreet";    // <-- Password for OTA uploads

/* ═══════════════════════════════════════════════════════════════════════════
 *                    SECTION 1: HARDWARE WIRING (PINS)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 *  These numbers tell the microcontroller which physical pin each LED strip
 *  is connected to. Only change these if you rewire the hardware.
 * 
 *  PIN GUIDE:
 *    D2  = GPIO 2  → Water Flow LEDs
 *    D4  = GPIO 4  → Water Data LEDs
 *    D18 = GPIO 18 → Electricity Flow LEDs
 *    D5  = GPIO 5  → Electricity Data LEDs
 */

#define PIN_WATER_FLOW   2      // Water Flow strip is plugged into pin D2
#define PIN_WATER_DATA   4      // Water Data strip is plugged into pin D4
#define PIN_ELEC_FLOW    18     // Electricity Flow strip is plugged into pin D18
#define PIN_ELEC_DATA    5      // Electricity Data strip is plugged into pin D5


/* ═══════════════════════════════════════════════════════════════════════════
 *                  SECTION 2: LED STRIP LENGTHS  ← CHANGE THIS
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 *  How many individual LEDs are in each strip?
 *  Count the LEDs on your physical strip and enter that number here.
 * 
 *  EXAMPLE: If your Water Flow strip has 25 LEDs, change 20 to 25
 */

#define NUM_WATER_FLOW_LEDS   20    // Water Flow has 20 LEDs
#define NUM_WATER_DATA_LEDS   5     // Water Data has 5 LEDs
#define NUM_ELEC_FLOW_LEDS    15    // Electricity Flow has 15 LEDs
#define NUM_ELEC_DATA_LEDS    7     // Electricity Data has 7 LEDs


/* ═══════════════════════════════════════════════════════════════════════════
 *                     SECTION 3: COLORS  ← CHANGE THIS
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 *  Colors use RGB format: CRGB(Red, Green, Blue)
 *  Each value goes from 0 (none) to 255 (maximum)
 * 
 *  COLOR MIXING GUIDE:
 *  ┌────────────────────────────────────────────────────────────────────────┐
 *  │  CRGB(255, 0, 0)      = Pure Red                                       │
 *  │  CRGB(0, 255, 0)      = Pure Green                                     │
 *  │  CRGB(0, 0, 255)      = Pure Blue                                      │
 *  │  CRGB(255, 255, 0)    = Yellow (Red + Green)                           │
 *  │  CRGB(255, 0, 255)    = Purple/Magenta (Red + Blue)                    │
 *  │  CRGB(0, 255, 255)    = Cyan (Green + Blue)                            │
 *  │  CRGB(255, 255, 255)  = White (all colors)                             │
 *  │  CRGB(255, 165, 0)    = Classic Orange                                 │
 *  │  CRGB(255, 90, 0)     = Deep Orange (current setting)                  │
 *  │  CRGB(0, 160, 200)    = Sea Blue (current setting)                     │
 *  └────────────────────────────────────────────────────────────────────────┘
 * 
 *  TIPS:
 *  - To make orange less red: increase the middle number (Green)
 *  - To make orange more red: decrease the middle number
 *  - To make blue more cyan: increase the Green value
 */

// Sea Blue color for Water Flow LEDs
// Current: R=0, G=160, B=200
#define COLOR_SEA_BLUE   CRGB(0, 160, 200)

// Orange color for all data/electricity LEDs
// Current: R=255, G=90, B=0
// If this looks too RED, try increasing 90 to 120 or 150
#define COLOR_ORANGE     CRGB(255, 90, 0)


/* ═══════════════════════════════════════════════════════════════════════════
 *                   SECTION 4: BRIGHTNESS  ← CHANGE THIS
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 *  Controls how bright ALL LED strips are.
 *  Range: 0 (completely off) to 255 (maximum brightness)
 * 
 *  RECOMMENDED VALUES:
 *  ┌──────────────────────────────────────┐
 *  │   50  = Dim (good for dark rooms)    │
 *  │  100  = Medium                       │
 *  │  150  = Bright (current setting)     │
 *  │  200  = Very bright                  │
 *  │  255  = Maximum (may be too harsh)   │
 *  └──────────────────────────────────────┘
 * 
 *  WARNING: Higher brightness = more power consumption
 */

#define BRIGHTNESS_LEVEL   150


/* ═══════════════════════════════════════════════════════════════════════════
 *                    SECTION 5: ANIMATION SPEED  ← CHANGE THIS
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 *  Speed is measured in MILLISECONDS (1000 milliseconds = 1 second)
 *  This controls how fast the glowing dot moves along each strip.
 * 
 *  SPEED GUIDE:
 *  ┌────────────────────────────────────────────────────────────────────────┐
 *  │   50 ms  = Very fast (hard to see)                                     │
 *  │  100 ms  = Fast                                                        │
 *  │  200 ms  = Medium-fast                                                 │
 *  │  350 ms  = Medium (current setting)                                    │
 *  │  500 ms  = Slow                                                        │
 *  │  750 ms  = Very slow                                                   │
 *  │ 1000 ms  = Extremely slow (1 second per LED)                           │
 *  └────────────────────────────────────────────────────────────────────────┘
 * 
 *  OPTION A: Set ONE speed for ALL strips (simple)
 *  OPTION B: Set DIFFERENT speeds for each strip (advanced)
 */

// ─── OPTION A: GLOBAL SPEED (affects all strips equally) ───
#define SPEED_MS_GLOBAL   350    // All strips move at this speed

// ─── OPTION B: INDIVIDUAL SPEEDS (uncomment to customize each strip) ───
// To use individual speeds, replace SPEED_MS_GLOBAL below with your own numbers
#define DELAY_WATER_FLOW   SPEED_MS_GLOBAL   // Try: 300 for faster water
#define DELAY_WATER_DATA   SPEED_MS_GLOBAL   // Try: 400 for slower data
#define DELAY_ELEC_FLOW    SPEED_MS_GLOBAL   // Try: 250 for fast electricity
#define DELAY_ELEC_DATA    SPEED_MS_GLOBAL   // Try: 400 for slower data


/* ═══════════════════════════════════════════════════════════════════════════
 *                  SECTION 6: ANIMATION BEHAVIOR  ← CHANGE THIS
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 *  CYCLES_PER_STAGE:
 *    How many times does the dot travel across the strip before moving
 *    to the next strip? 
 *    Current: 4 (the dot goes back and forth 4 times)
 * 
 *  PAUSE_BETWEEN_STAGES:
 *    How long to wait (in milliseconds) after one strip finishes 
 *    before the next strip starts?
 *    Current: 500ms (half a second pause)
 * 
 *  PAUSE_AFTER_FULL_CYCLE:
 *    How long to wait after ALL 4 strips have performed before 
 *    starting over from the beginning?
 *    Current: 3000ms (3 seconds of darkness)
 */

#define CYCLES_PER_STAGE        4       // Number of passes per strip
#define PAUSE_BETWEEN_STAGES    500     // Pause between strips (ms)
#define PAUSE_AFTER_FULL_CYCLE  3000    // Pause after all 4 strips done (ms)


/* ═══════════════════════════════════════════════════════════════════════════
 * ═══════════════════════════════════════════════════════════════════════════
 *                    ⚠️  ADVANCED SECTION - DO NOT MODIFY  ⚠️
 *              (Unless you understand programming concepts)
 * ═══════════════════════════════════════════════════════════════════════════
 * ═══════════════════════════════════════════════════════════════════════════
 */

// LED Arrays - memory storage for each strip's color data
CRGB waterFlowLeds[NUM_WATER_FLOW_LEDS];
CRGB waterDataLeds[NUM_WATER_DATA_LEDS];
CRGB elecFlowLeds[NUM_ELEC_FLOW_LEDS];
CRGB elecDataLeds[NUM_ELEC_DATA_LEDS];

/*
 * clearStrip() - Turns off all LEDs in a single strip
 * Used to reset a strip before lighting up the next LED position
 */
void clearStrip(CRGB* leds, int numLeds) {
  for (int i = 0; i < numLeds; i++) {
    leds[i] = CRGB::Black;  // Black = LED off
  }
}

/*
 * clearAllLeds() - Turns off ALL LED strips at once
 * Called at startup and between full animation cycles
 */
void clearAllLeds() {
  clearStrip(waterFlowLeds, NUM_WATER_FLOW_LEDS);
  clearStrip(waterDataLeds, NUM_WATER_DATA_LEDS);
  clearStrip(elecFlowLeds, NUM_ELEC_FLOW_LEDS);
  clearStrip(elecDataLeds, NUM_ELEC_DATA_LEDS);
  FastLED.show();  // Send the "off" command to the hardware
}

/*
 * oneDotPass() - Animates a single glowing dot moving across a strip
 * 
 * Parameters:
 *   leds     = Which LED strip to animate
 *   numLeds  = How many LEDs in that strip
 *   color    = What color the dot should be
 *   reverse  = false = left-to-right, true = right-to-left
 *   delayMs  = How many milliseconds to wait between each LED
 */
void oneDotPass(CRGB* leds, int numLeds, CRGB color, bool reverse, int delayMs) {
  if (!reverse) {
    // Forward direction: LED 0 → LED 1 → LED 2 → ... → Last LED
    for (int pos = 0; pos < numLeds; pos++) {
      clearStrip(leds, numLeds);   // Turn off all LEDs first
      leds[pos] = color;           // Light up current position
      FastLED.show();              // Send to hardware
      delay(delayMs);              // Wait before moving to next
      ArduinoOTA.handle();         // Check for OTA updates
    }
  } else {
    // Reverse direction: Last LED → ... → LED 2 → LED 1 → LED 0
    for (int pos = numLeds - 1; pos >= 0; pos--) {
      clearStrip(leds, numLeds);
      leds[pos] = color;
      FastLED.show();
      delay(delayMs);
      ArduinoOTA.handle();         // Check for OTA updates
    }
  }
}

/*
 * runStage() - Runs the complete animation for one LED strip
 * 
 * This is like one "actor" performing their part on stage:
 *   1. The dot travels across the strip multiple times (CYCLES_PER_STAGE)
 *   2. Then the strip turns off
 *   3. Then there's a pause before the next strip starts
 */
void runStage(CRGB* leds, int numLeds, CRGB color, bool reverse, int delayMs) {
  // Repeat the animation for the specified number of cycles
  for (int cycle = 0; cycle < CYCLES_PER_STAGE; cycle++) {
    oneDotPass(leds, numLeds, color, reverse, delayMs);
  }
  // Turn off this strip when done
  clearStrip(leds, numLeds);
  FastLED.show();
  // Pause before next strip starts
  delay(PAUSE_BETWEEN_STAGES);
}

/*
 * setup() - Runs ONCE when the microcontroller powers on
 * 
 * This initializes all the LED strips and tells the FastLED library
 * which pins to use and what color order each strip expects.
 * 
 * COLOR ORDER EXPLANATION:
 *   Different LED strips expect colors in different orders.
 *   - BGR = Blue-Green-Red (Water Flow strip)
 *   - RGB = Red-Green-Blue (Water Data, Electricity Data strips)
 *   - GRB = Green-Red-Blue (Electricity Flow strip)
 *   
 *   If colors look wrong (e.g., blue when you expect red), the color
 *   order setting might be incorrect for that strip.
 */
void setup() {
  // Set overall brightness for all strips
  FastLED.setBrightness(BRIGHTNESS_LEVEL);

  // Initialize each LED strip with its pin and color order
  FastLED.addLeds<WS2812B, PIN_WATER_FLOW, BGR>(waterFlowLeds, NUM_WATER_FLOW_LEDS);
  FastLED.addLeds<WS2812B, PIN_WATER_DATA, RGB>(waterDataLeds, NUM_WATER_DATA_LEDS);
  FastLED.addLeds<WS2812B, PIN_ELEC_FLOW,  GRB>(elecFlowLeds,  NUM_ELEC_FLOW_LEDS);
  FastLED.addLeds<WS2812B, PIN_ELEC_DATA,  RGB>(elecDataLeds,  NUM_ELEC_DATA_LEDS);

  // Start with all LEDs off
  clearAllLeds();
  
  // Connect to WiFi for OTA updates
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);
  
  // Wait for connection (10 second timeout - don't block animations too long)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }
  
  // Initialize OTA if WiFi connected
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.setHostname(otaHostname);
    ArduinoOTA.setPassword(otaPassword);
    ArduinoOTA.begin();
  }
}

/*
 * loop() - Runs FOREVER after setup() completes
 * 
 * This is the main animation sequence. It runs each LED strip
 * one after another, like actors taking turns on a stage:
 * 
 *   ACT 1: Water Flow    (Sea Blue, moves forward →)
 *   ACT 2: Water Data    (Orange, moves backward ←)
 *   ACT 3: Electricity Flow (Orange, moves forward →)
 *   ACT 4: Electricity Data (Orange, moves backward ←)
 *   
 *   Then all lights go dark for a few seconds, and it repeats.
 */
void loop() {
  // Handle OTA updates
  ArduinoOTA.handle();
  
  // ═══ ACT 1: WATER FLOW ═══
  // Sea blue dot moves forward (left to right)
  // Represents physical water flowing through the meters
  runStage(waterFlowLeds, NUM_WATER_FLOW_LEDS, COLOR_SEA_BLUE, false, DELAY_WATER_FLOW);

  // ═══ ACT 2: WATER DATA ═══
  // Orange dot moves backward (right to left)
  // Represents data being sent from water meters to the logger
  runStage(waterDataLeds, NUM_WATER_DATA_LEDS, COLOR_ORANGE, true, DELAY_WATER_DATA);

  // ═══ ACT 3: ELECTRICITY FLOW ═══
  // Orange dot moves forward (left to right)
  // Represents electric current flowing in the main line
  runStage(elecFlowLeds, NUM_ELEC_FLOW_LEDS, COLOR_ORANGE, false, DELAY_ELEC_FLOW);

  // ═══ ACT 4: ELECTRICITY DATA ═══
  // Orange dot moves backward (right to left)
  // Represents electrical measurement data sent to the logger
  runStage(elecDataLeds, NUM_ELEC_DATA_LEDS, COLOR_ORANGE, true, DELAY_ELEC_DATA);

  // ═══ INTERMISSION ═══
  // All lights go off, pause, then the show starts again
  clearAllLeds();
  delay(PAUSE_AFTER_FULL_CYCLE);
}
