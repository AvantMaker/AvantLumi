/*
 * AvantLumi - Basic Control Demo (RGB and Color Names)
 * 
 * Description:
 * This example demonstrates how to set solid colors on an LED strip using the 
 * AvantLumi library. It covers two primary methods: setting colors with specific 
 * RGB values using setRGB(), and setting colors with predefined names (e.g., "red", 
 * "LightGreen") using setColor().
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
 * It will cycle through a predefined sequence of solid colors to showcase
 * the library's color control capabilities. Monitor the serial output at 
 * 115200 baud to see status messages for each step.
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
    SET_RGB_RED,
    SET_RGB_GREEN,
    SET_RGB_BLUE,
    SET_NAME_ORANGE,
    SET_NAME_PURPLE,
    SET_NAME_DEEPSKYBLUE,
    PAUSE_BEFORE_RESTART
};

DemoStep currentStep = SET_RGB_RED;
unsigned long lastStepTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("AvantLumi - Basic Color Control Demo");

    // Initialize the LED controller
    if (myLumi.begin()) {
        Serial.println("AvantLumi initialized successfully!");
    } else {
        Serial.println("AvantLumi initialization failed! Halting.");
        while (true); // Stop execution
    }

    // Set initial brightness and disable fade for this demo to make
    // the solid colors clear and distinct.
    myLumi.setBright(3);
    myLumi.setFade(false);
    
    Serial.println("Starting demo...");
    lastStepTime = millis();
}

void loop() {
    // CRITICAL: You must call the update() method in your main loop.
    // This handles all the background work and updates the LED strip.
    myLumi.update();

    // --- Demonstration Logic ---
    // This section controls the sequence of the demo, cycling every 3 seconds.
    if (millis() - lastStepTime < 3000) {
        return; // Wait for 3 seconds before the next step
    }
    lastStepTime = millis();

    switch (currentStep) {
        case SET_RGB_RED:
            Serial.println("\n1. Setting color using setRGB(255, 0, 0)");
            
            // Use setRGB() to specify a color with Red, Green, and Blue values (0-255).
            // This sets the entire strip to a solid red color.
            myLumi.setRGB(255, 0, 0);
            
            currentStep = SET_RGB_GREEN;
            break;

        case SET_RGB_GREEN:
            Serial.println("2. Setting color using setRGB(0, 255, 0)");
            myLumi.setRGB(0, 255, 0); // Solid Green
            currentStep = SET_RGB_BLUE;
            break;

        case SET_RGB_BLUE:
            Serial.println("3. Setting color using setRGB(0, 0, 255)");
            myLumi.setRGB(0, 0, 255); // Solid Blue
            currentStep = SET_NAME_ORANGE;
            break;

        case SET_NAME_ORANGE:
            Serial.println("\n4. Setting color using setColor(\"orange\")");

            // Use setColor() to specify a color by its name.
            // The library recognizes a wide range of standard color names.
            // The names are case-insensitive.
            myLumi.setColor("orange");
            
            currentStep = SET_NAME_PURPLE;
            break;

        case SET_NAME_PURPLE:
            Serial.println("5. Setting color using setColor(\"purple\")");
            myLumi.setColor("purple");
            currentStep = SET_NAME_DEEPSKYBLUE;
            break;

        case SET_NAME_DEEPSKYBLUE:
            Serial.println("6. Setting color using setColor(\"DeepSkyBlue\")");
            myLumi.setColor("DeepSkyBlue"); // Note: Case doesn't matter
            currentStep = PAUSE_BEFORE_RESTART;
            break;

        case PAUSE_BEFORE_RESTART:
            Serial.println("\n--- Restarting Demo ---");
            currentStep = SET_RGB_RED;
            break;
    }
}

/*
 * --- Comprehensive List of Supported Color Names for setColor() ---
 * 
 * You can use any of the following string names in the setColor() method.
 * The names are not case-sensitive (e.g., "red" is the same as "Red").
 * 
 * Basic Colors:
 * "red", "green", "blue", "white", "black", "yellow", "cyan", "magenta", 
 * "orange", "purple", "pink", "brown", "lime", "aqua", "silver", "gold",
 * "gray", "grey", "teal"
 * 
 * Light Variations:
 * "lightgreen", "lightblue", "lightpink", "lightcyan", "lightyellow", 
 * "lightsteelblue"
 * 
 * Dark Variations:
 * "darkred", "darkgreen", "darkblue", "darkorange", "darkviolet", 
 * "darkgray", "darkgrey", "darkcyan", "darkgoldenrod", "darkslateblue", 
 * "darkturquoise", "darkseagreen", "darkslategray", "darkslategrey"
 * 
 * Medium Variations:
 * "mediumblue", "mediumorchid", "mediumspringgreen"
 * 
 * Greens & Blues:
 * "springgreen", "forestgreen", "seagreen", "limegreen", "aquamarine", 
 * "turquoise", "palegreen", "paleturquoise", "powderblue", "steelblue", 
 * "royalblue", "cornflowerblue", "deepskyblue", "dodgerblue", "midnightblue", 
 * "navy", "cadetblue"
 * 
 * Pinks, Reds & Oranges:
 * "indigo", "violet", "fuchsia", "hotpink", "deeppink", "crimson", 
 * "firebrick", "maroon", "orangered", "tomato", "coral", "chocolate", 
 * "saddlebrown", "sienna", "peru", "indianred"
 * 
 * Yellows & Whites:
 * "goldenrod", "greenyellow", "lemonchiffon", "honeydew", "lavender", 
 * "thistle", "aliceblue"
 */
