/*
 * AvantLumi - Advanced Control (Customizing Palettes)
 * 
 * Description:
 * This example explains how to add your own custom color palettes to the 
 * AvantLumi library. While the code simply displays an existing custom palette,
 * the detailed comments in this file serve as a guide to modifying the library's
 * source files (`AvantLumi.h` and `AvantLumi.cpp`) to create and use your own
 * unique color schemes.
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
 * This sketch will run automatically and display the "fire" (u10) custom palette.
 * The primary purpose of this file is to provide instructions. Read the comments
 * carefully to learn how to create your own palettes.
 * 
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

#include "AvantLumi.h"

// LED Configuration
#define DATA_PIN 2
#define NUM_LEDS 17

// Create an instance of the AvantLumi library
AvantLumi myLumi(DATA_PIN, NUM_LEDS);

void setup() {
    Serial.begin(115200);
    Serial.println("AvantLumi - Custom Palette Guide");

    // Initialize the LED controller
    if (myLumi.begin()) {
        Serial.println("AvantLumi initialized successfully!");
    } else {
        Serial.println("AvantLumi initialization failed! Halting.");
        while (true); // Stop execution
    }

    myLumi.setBright(4);
    myLumi.setFade(true);
    
    Serial.println("\nThis demo is now displaying the 'fire' (u10) custom palette.");
    Serial.println("Please read the detailed comments in this sketch to learn how to create your own!");

    // To demonstrate, we are setting one of the existing custom palettes.
    // After you create your own, you would call it here by its new name.
    myLumi.setPalette("fire");
}

void loop() {
    // CRITICAL: You must call the update() method in your main loop.
    // This handles all the background work for the LEDs.
    myLumi.update();
}

/*
 * --- How to Create Your Own Custom Palettes ---
 * 
 * Ready to create your own unique color themes? This guide will walk you through
 * modifying the AvantLumi library to add your own custom palettes. You will need
 * to edit two files: `AvantLumi.h` and `AvantLumi.cpp`.
 * 
 * --- STEP 1: Declare Your New Palette in `AvantLumi.h` ---
 * 
 * 1. Open the `AvantLumi.h` file in your editor.
 * 2. Navigate to the `private:` section of the `AvantLumi` class.
 * 3. Find the list of custom palette definitions that look like this:
 *    static const CRGBPalette16 christmas_p;
 *    static const CRGBPalette16 autumn_p;
 *    ...
 * 4. Add a new line for your palette. It's best to follow the naming convention
 *    by ending the variable name with `_p`. For example:
 *    static const CRGBPalette16 my_awesome_palette_p;
 * 
 * 
 * --- STEP 2: Define Your New Palette in `AvantLumi.cpp` ---
 * 
 * 1. Open the `AvantLumi.cpp` file in your editor.
 * 2. Find the section at the top of the file where the custom palettes are defined.
 *    You will see blocks of code like `const CRGBPalette16 AvantLumi::christmas_p = ...`
 * 3. Add the definition for your new palette below the existing ones. A `CRGBPalette16`
 *    is made of 16 `CRGB` colors. You can use predefined FastLED colors (e.g., `CRGB::Red`)
 *    or define your own with RGB values (e.g., `CRGB(100, 50, 200)`).
 * 
 *    Example Definition:
 *    const CRGBPalette16 AvantLumi::my_awesome_palette_p = CRGBPalette16(
 *        CRGB::DeepSkyBlue, CRGB::Black, CRGB::Aqua, CRGB::Black,
 *        CRGB::DeepSkyBlue, CRGB::Black, CRGB::Aqua, CRGB::Black,
 *        CRGB::DeepSkyBlue, CRGB::Black, CRGB::Aqua, CRGB::Black,
 *        CRGB::DeepSkyBlue, CRGB::Black, CRGB::Aqua, CRGB::Black
 *    );
 * 
 * 
 * --- STEP 3: Make Your Palette Usable in `AvantLumi.cpp` ---
 * 
 * 1. In `AvantLumi.cpp`, find the `setPalette(String paletteName)` method.
 * 2. Inside this method, you will see a series of `else if` statements that check
 *    the `paletteName` and assign the `targetPalette`.
 * 3. Add a new `else if` block for your palette. You can assign it a unique name
 *    (e.g., "awesome") and a user code (e.g., "u11").
 * 
 *    Example Addition:
 *    ...
 *    else if (paletteName == "u10" || paletteName == "fire") {
 *        targetPalette = fire_p;
 *        useRandomPalette = false;
 *        currentPaletteName = "u10_fire";
 *    }
 *    // --- ADD YOUR NEW BLOCK HERE ---
 *    else if (paletteName == "u11" || paletteName == "awesome") {
 *        targetPalette = my_awesome_palette_p; // Use the variable from Step 1 & 2
 *        useRandomPalette = false;
 *        currentPaletteName = "u11_awesome";   // Give it a status name
 *    }
 *    // --- END OF YOUR NEW BLOCK ---
 *    else if (paletteName == "random") {
 *    ...
 * 
 * --- STEP 4: Use Your New Palette! ---
 * 
 * That's it! After saving the changes to `AvantLumi.h` and `AvantLumi.cpp`, you
 * can now use your new palette in any sketch by calling:
 * 
 * myLumi.setPalette("awesome"); 
 * // or
 * myLumi.setPalette("u11");
 * 
 */
