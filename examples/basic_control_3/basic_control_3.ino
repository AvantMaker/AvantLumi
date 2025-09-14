/*
 * AvantLumi - Basic Control Demo (Palettes)
 * 
 * Description:
 * This example demonstrates how to use color palettes with the AvantLumi library.
 * It shows how to apply both built-in FastLED palettes (like "rainbow") and the 
 * custom-themed palettes included with the library (like "christmas" or "u01").
 * The demo also shows how to enable the random palette generator.
 * 
 * Author: AvantMaker <admin@avantmaker.com>
 * Author Website: https://www.AvantMaker.com
 * Date: September 1, 2025
 * Version: 1.0.0
 * 
 * Hardware Requirements:
 * - ESP32-based microcontroller (e.g., ESP32 DevKitC, DOIT ESP32 DevKit, etc.)
 * 
 * Dependencies:
 * - FastLED library (available at https://github.com/FastLED/FastLED)
 * 
 * License: MIT License
 * Repository: https://github.com/AvantMaker/avantlumi
 * 
 * Usage Notes:
 * This demo runs automatically and does not require any user interaction.
 * It will cycle through a predefined sequence of palettes to showcase
 * the library's capabilities. Monitor the serial output at 115200 baud
 * to see status messages for each step.
 * 
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

#include "AvantLumi.h"

// LED Configuration
#define DATA_PIN 2
#define NUM_LEDS 17

// Create an instance of the AvantLumi library
AvantLumi myLumi(DATA_PIN, NUM_LEDS);

// Use an enum to manage the steps of our demonstration
enum DemoStep {
    SET_PALETTE_RAINBOW,
    SET_PALETTE_OCEAN,
    SET_PALETTE_CHRISTMAS,
    SET_PALETTE_CYBERPUNK,
    SET_PALETTE_RANDOM,
    PAUSE_BEFORE_RESTART
};

DemoStep currentStep = SET_PALETTE_RAINBOW;
unsigned long lastStepTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("AvantLumi - Basic Palette Control Demo");

    // Initialize the LED controller
    if (myLumi.begin()) {
        Serial.println("AvantLumi initialized successfully!");
    } else {
        Serial.println("AvantLumi initialization failed! Halting.");
        while (true); // Stop execution
    }

    // Set initial brightness and ensure fade is on for this demo
    // to best showcase the smooth palette transitions.
    myLumi.setBright(3);
    myLumi.setFade(true);
    
    Serial.println("Starting demo...");
    lastStepTime = millis();
}

void loop() {
    // CRITICAL: You must call the update() method in your main loop.
    // This handles all the background work like palette blending,
    // fade effects, and updating the LED strip.
    myLumi.update();

    // --- Demonstration Logic ---
    // This section controls the sequence of the demo, cycling every 5 seconds.
    if (millis() - lastStepTime < 5000) {
        return; // Wait for 5 seconds before the next step
    }
    lastStepTime = millis();

    switch (currentStep) {
        case SET_PALETTE_RAINBOW:
            Serial.println("\n1. Setting palette to 'rainbow'");
            
            // Use setPalette() to apply a color palette by name.
            // "rainbow" is a classic built-in FastLED palette.
            myLumi.setPalette("rainbow");
            
            currentStep = SET_PALETTE_OCEAN;
            break;

        case SET_PALETTE_OCEAN:
            Serial.println("2. Setting palette to 'ocean'");
            
            // "ocean" is another built-in palette with cool blue and white tones.
            myLumi.setPalette("ocean");
            
            currentStep = SET_PALETTE_CHRISTMAS;
            break;

        case SET_PALETTE_CHRISTMAS:
            Serial.println("3. Setting custom palette to 'christmas' (u01)");

            // This is one of the custom-themed palettes defined in the library.
            // You can refer to it by its name ("christmas") or its code ("u01").
            myLumi.setPalette("christmas");
            
            currentStep = SET_PALETTE_CYBERPUNK;
            break;

        case SET_PALETTE_CYBERPUNK:
            Serial.println("4. Setting custom palette to 'cyberpunk' (u03)");
            
            // Another custom palette with a futuristic feel.
            myLumi.setPalette("u03"); // Using the code "u03" this time.
            
            currentStep = SET_PALETTE_RANDOM;
            break;

        case SET_PALETTE_RANDOM:
            Serial.println("5. Setting to 'random' palette mode");

            // The "random" mode tells the library to automatically generate
            // and transition to new, random palettes every few seconds.
            myLumi.setPalette("random");
            
            currentStep = PAUSE_BEFORE_RESTART;
            break;

        case PAUSE_BEFORE_RESTART:
            Serial.println("\n--- Restarting Demo ---");
            currentStep = SET_PALETTE_RAINBOW;
            break;
    }
}

/*
 * --- Comprehensive List of Supported Palette Names for setPalette() ---
 * 
 * You can use any of the following string names in the setPalette() method.
 * The names are not case-sensitive.
 * 
 * Built-in FastLED Palettes:
 * "rainbow", "party", "ocean", "forest", "heat", "cloud", "lava"
 * 
 * Custom Themed Palettes (Name or Code):
 * - "christmas"  or "u01"  (Red, Green, White)
 * - "autumn"     or "u02"  (Orange, Brown, Red)
 * - "cyberpunk"  or "u03"  (Magenta, Pink, Cyan)
 * - "halloween"  or "u04"  (Orange, Purple, Black)
 * - "winter"     or "u05"  (Blue, White, Light Blue)
 * - "spring"     or "u06"  (Light Green, Pink, Yellow)
 * - "sunset"     or "u07"  (Red, Orange, Yellow)
 * - "deep_ocean" or "u08"  (Deep Blues, Teals)
 * - "neon"       or "u09"  (Bright Lime, Magenta, Cyan)
 * - "fire"       or "u10"  (Red, Orange, Yellow Fire)
 * 
 * Special Modes:
 * "random" - Activates the automatic random palette generator. The library
 *            will create and blend to new palettes on its own.
 */
