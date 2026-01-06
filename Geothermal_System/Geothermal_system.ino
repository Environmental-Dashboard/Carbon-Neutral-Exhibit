/*
 * ============================================================================
 * GEOTHERMAL SYSTEM - LED VISUALIZATION
 * ============================================================================
 * 
 * Part of the Carbon Neutral Exhibition project.
 * Environmental Dashboard - https://github.com/Environmental-Dashboard
 * 
 * DESCRIPTION:
 * This code visualizes how a geothermal heat pump works in both SUMMER
 * (cooling) and WINTER (heating) modes. A single "globule" travels along
 * the LED strip, changing color to represent heat transfer:
 * 
 *   SUMMER MODE: Heat is extracted FROM the building and dumped INTO ground
 *     - Starts RED (hot from building) → transitions → ends BLUE (cold to ground)
 *   
 *   WINTER MODE: Heat is extracted FROM the ground and pumped INTO building  
 *     - Starts BLUE (cold from ground) → transitions → ends RED (hot to building)
 * 
 * The system alternates between 4 summer cycles and 4 winter cycles,
 * with indicator LEDs showing which mode is active.
 * 
 * ============================================================================
 * HARDWARE REQUIREMENTS
 * ============================================================================
 * 
 * COMPONENT              SPECIFICATION
 * ─────────────────────────────────────────────────────────────────────────────
 * Microcontroller        ESP32 Dev Module
 * LED Strip              WS2812B addressable LEDs (44 LEDs) - 24V
 * Power Supply           24V main supply (for LED strip)
 * Step-Down Converter    24V → 5V DC-DC converter (for ESP32 & relay)
 * Relay Module           2-channel relay module (for summer/winter signs)
 * Summer Sign            Connected via relay on GPIO 4 (D4)
 * Winter Sign            Connected via relay on GPIO 5 (D5)
 * 
 * ============================================================================
 * LIBRARY REQUIRED
 * ============================================================================
 * 
 * FastLED - Install via Arduino IDE:
 *   Sketch → Include Library → Manage Libraries → Search "FastLED" → Install
 * 
 * ============================================================================
 * HOW TO CUSTOMIZE - QUICK REFERENCE
 * ============================================================================
 * 
 * SETTING              WHERE TO CHANGE              EXAMPLE
 * ─────────────────────────────────────────────────────────────────────────────
 * Animation speed      speedMs = 200                Lower = faster
 * Number of LEDs       NUM_LEDS = 44                Match your strip
 * LED data pin         LED_PIN = 2                  Any GPIO pin
 * Brightness           BRIGHTNESS = 150             0-255
 * Summer indicator     SUMMER_PIN = 4               GPIO for LED/relay
 * Winter indicator     WINTER_PIN = 5               GPIO for LED/relay
 * Color order          COLOR_ORDER = RGB            Try GRB if colors wrong
 * Segment distances    d1, d2, d3...                LEDs per color zone
 * Colors               S_RED, S_ORANGE, etc.        CRGB(R, G, B)
 * Cycles per mode      for (int i = 0; i < 4...)    Change 4 to any number
 * 
 * ============================================================================
 */

#include <FastLED.h>

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================
// Adjust these values to match your physical setup

#define LED_PIN     2         // GPIO pin connected to LED strip data line
                              // Common ESP32 pins: 2, 4, 5, 12, 13, 14, 15

#define NUM_LEDS    44        // Total number of LEDs in your strip
                              // Count your LEDs and update this value

#define LED_TYPE    WS2812B   // Type of LED chip
                              // Options: WS2812B, WS2811, WS2813, SK6812, NEOPIXEL

#define COLOR_ORDER RGB       // Color order of your LED strip
                              // If colors look wrong, try: GRB, BGR, RBG

#define BRIGHTNESS  150       // Master brightness (0-255)
                              // 50=dim, 150=bright, 255=maximum
                              // Higher values use more power

CRGB leds[NUM_LEDS];          // Array to hold LED color data

// ============================================================================
// INDICATOR PIN CONFIGURATION
// ============================================================================
// These pins control external LEDs or relays to show current mode

#define SUMMER_PIN  4         // GPIO 4 (D4) - HIGH when in summer mode
#define WINTER_PIN  5         // GPIO 5 (D5) - HIGH when in winter mode

// ============================================================================
// ANIMATION SPEED
// ============================================================================

int speedMs = 200;            // Delay between each LED step (milliseconds)
                              // 50-100  = fast (energetic)
                              // 150-250 = medium (default, easy to follow)
                              // 300-500 = slow (dramatic, meditative)

// ============================================================================
// SEGMENT DISTANCES
// ============================================================================
// These define how many LEDs the globule travels in each color phase.
// The globule changes color after traveling the specified number of LEDs.
// 
// PHASE SEQUENCE (10 phases total):
//   Phase 0-4: Outbound journey (building → ground in summer)
//   Phase 5-9: Return journey (ground → building in summer)
//
// Total LEDs = d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + d10
// Current total = 5 + 5 + 7 + 5 + 5 + 2 + 3 + 2 + 3 + 2 = 39 LEDs

#define d1   5    // Phase 0: RED zone        (hot start)
#define d2   5    // Phase 1: ORANGE zone     (warming)
#define d3   7    // Phase 2: WHITE zone      (heat exchanger)
#define d4   5    // Phase 3: LIGHT BLUE zone (cooling)
#define d5   5    // Phase 4: BLUE zone       (cold end)
#define d6   2    // Phase 5: BLUE zone       (return start)
#define d7   3    // Phase 6: LIGHT BLUE zone (return warming)
#define d8   2    // Phase 7: WHITE zone      (return neutral)
#define d9   3    // Phase 8: ORANGE zone     (return warming)
#define d10  2    // Phase 9: RED zone        (return hot end)

// ============================================================================
// COLOR DEFINITIONS (SUMMER MODE)
// ============================================================================
// These colors represent the "summer meaning" - what the colors mean
// when the system is in cooling mode (summer).
//
// Format: CRGB(Red, Green, Blue) - each value 0-255
// Or use predefined colors: CRGB::Red, CRGB::Blue, etc.

#define S_RED        CRGB::Red              // Hot - heat from building
                                            // Full red: (255, 0, 0)

#define S_ORANGE     CRGB(255, 120, 0)      // Warm - transitional heat
                                            // Adjust green for more/less yellow

#define S_WHITE      CRGB(255, 220, 200)    // Neutral - heat exchanger zone
                                            // Slight warm tint for visibility

#define S_LIGHTBLUE  CRGB(0, 80, 255)       // Cool - transitional cold
                                            // Darker blue for contrast

#define S_BLUE       CRGB::Blue             // Cold - dumped into ground
                                            // Full blue: (0, 0, 255)

// ============================================================================
// winterColor() - CONVERTS SUMMER COLORS TO WINTER COLORS
// ============================================================================
// In winter mode, the heat flow is REVERSED:
//   - What was HOT in summer becomes COLD in winter
//   - What was COLD in summer becomes HOT in winter
//
// This function swaps the colors to represent the reversed flow:
//   Summer RED    (hot from building)  → Winter BLUE   (cold from ground)
//   Summer ORANGE (warm)               → Winter LIGHTBLUE (cool)
//   Summer WHITE  (neutral)            → Winter WHITE  (neutral - no change)
//   Summer LIGHTBLUE (cool)            → Winter ORANGE (warm)
//   Summer BLUE   (cold to ground)     → Winter RED    (hot to building)

CRGB winterColor(CRGB c) {
  if (c == S_BLUE)      return S_RED;       // Cold becomes Hot
  if (c == S_LIGHTBLUE) return S_ORANGE;    // Cool becomes Warm
  if (c == S_WHITE)     return S_WHITE;     // Neutral stays Neutral
  if (c == S_ORANGE)    return S_LIGHTBLUE; // Warm becomes Cool
  if (c == S_RED)       return S_BLUE;      // Hot becomes Cold
  return c;  // Default: return unchanged
}

// ============================================================================
// runGlobuleCycle() - RUNS ONE COMPLETE GLOBULE ANIMATION
// ============================================================================
// This function animates a single "globule" traveling from LED 0 to LED 43.
// The globule changes color as it moves through different zones.
//
// PARAMETERS:
//   winterMode - false = summer colors, true = winter colors (reversed)
//
// HOW IT WORKS:
//   1. Start at position 0 with phase 0 (RED in summer)
//   2. Light one LED with the current color
//   3. Move to next position
//   4. After traveling the distance for current phase, advance to next phase
//   5. Each phase has a different color and distance
//   6. Repeat until globule reaches end of strip

void runGlobuleCycle(bool winterMode) {
  int pos = 0;      // Current LED position (0 to NUM_LEDS-1)
  int step = 0;     // Steps taken in current color phase
  int phase = 0;    // Current color phase (0-9)

  // Continue until globule reaches end of strip
  while (pos < NUM_LEDS) {

    // Determine color and distance for current phase
    CRGB summerMeaning = CRGB::Black;
    int maxSteps = 1;

    switch (phase) {
      // ===== OUTBOUND JOURNEY (phases 0-4) =====
      case 0: summerMeaning = S_RED;       maxSteps = d1;  break;  // Hot start
      case 1: summerMeaning = S_ORANGE;    maxSteps = d2;  break;  // Warming
      case 2: summerMeaning = S_WHITE;     maxSteps = d3;  break;  // Exchanger
      case 3: summerMeaning = S_LIGHTBLUE; maxSteps = d4;  break;  // Cooling
      case 4: summerMeaning = S_BLUE;      maxSteps = d5;  break;  // Cold end
      
      // ===== RETURN JOURNEY (phases 5-9) =====
      case 5: summerMeaning = S_BLUE;      maxSteps = d6;  break;  // Return cold
      case 6: summerMeaning = S_LIGHTBLUE; maxSteps = d7;  break;  // Return cool
      case 7: summerMeaning = S_WHITE;     maxSteps = d8;  break;  // Return neutral
      case 8: summerMeaning = S_ORANGE;    maxSteps = d9;  break;  // Return warm
      case 9: summerMeaning = S_RED;       maxSteps = d10; break;  // Return hot
    }

    // Apply winter color swap if in winter mode
    CRGB currentColor = winterMode
                        ? winterColor(summerMeaning)
                        : summerMeaning;

    // Clear all LEDs and light only current position
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    leds[pos] = currentColor;
    FastLED.show();

    // Wait before moving to next position
    delay(speedMs);

    // Move to next position
    pos++;
    step++;

    // Check if we've completed this color phase
    if (step >= maxSteps) {
      step = 0;       // Reset step counter
      phase++;        // Move to next phase
      if (phase > 9) {
        phase = 0;    // Wrap back to phase 0 if needed
      }
    }
  }
}

// ============================================================================
// setup() - RUNS ONCE AT STARTUP
// ============================================================================

void setup() {
  // Initialize LED strip
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Configure indicator pins as outputs
  pinMode(SUMMER_PIN, OUTPUT);
  pinMode(WINTER_PIN, OUTPUT);

  // Turn off both indicators initially
  digitalWrite(SUMMER_PIN, LOW);
  digitalWrite(WINTER_PIN, LOW);

  // Clear LED strip (all off)
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(100);  // Brief pause for stable startup
}

// ============================================================================
// loop() - MAIN PROGRAM LOOP (runs forever)
// ============================================================================
// Alternates between summer and winter modes:
//   - 4 complete globule cycles in SUMMER mode
//   - 4 complete globule cycles in WINTER mode
//   - Repeat forever
//
// Indicator pins show which mode is currently active.

void loop() {

  // ==================== SUMMER MODE ====================
  // Simulate cooling: heat extracted from building, dumped to ground
  // Globule goes: RED → ORANGE → WHITE → LIGHTBLUE → BLUE
  
  digitalWrite(SUMMER_PIN, HIGH);   // Turn ON summer indicator
  digitalWrite(WINTER_PIN, LOW);    // Turn OFF winter indicator

  for (int i = 0; i < 4; i++) {     // Run 4 cycles
    runGlobuleCycle(false);         // false = summer mode
  }

  // ==================== WINTER MODE ====================
  // Simulate heating: heat extracted from ground, pumped to building
  // Globule goes: BLUE → LIGHTBLUE → WHITE → ORANGE → RED
  
  digitalWrite(SUMMER_PIN, LOW);    // Turn OFF summer indicator
  digitalWrite(WINTER_PIN, HIGH);   // Turn ON winter indicator

  for (int i = 0; i < 4; i++) {     // Run 4 cycles
    runGlobuleCycle(true);          // true = winter mode
  }
}

// ============================================================================
// END OF FILE
// ============================================================================
