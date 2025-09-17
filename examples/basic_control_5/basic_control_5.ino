/*
 * AvantLumi - Basic Control 5: Blend Speed, Power, and Status
 * 
 * Description:
 * This example demonstrates how to configure advanced settings in the
 * AvantLumi library, including setting the blend speed for palette
 * transitions, configuring maximum power usage, and retrieving the
 * current status of the LED controller.
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
 * It will cycle through different blend speeds and report status changes.
 * Monitor the serial output at 115200 baud to see status messages and
 * observe the effects of changing blend speeds.
 */

#include "AvantLumi.h"

// LED Configuration
#define DATA_PIN 2
#define NUM_LEDS 17

// Create AvantLumi object
AvantLumi myLumi(DATA_PIN, NUM_LEDS);

void setup() {
    Serial.begin(115200);
    Serial.println("AvantLumi Library: Advanced Control Demo");

    // 1. Set Maximum Power
    // It's crucial to set a power limit to protect your hardware (LEDs and power supply).
    // FastLED will estimate power consumption and cap it at this limit to prevent damage.
    // Here, we set it to 5 Volts and 800 Milliamps. Adjust this to your hardware.
    Serial.println("\n1. Setting max power to 5V, 800mA...");
    if (myLumi.setMaxPower(5, 800)) {
        Serial.println("Power limit set successfully.");
    } else {
        Serial.println("Failed to set power limit.");
    }

    // Initialize the LED controller
    if (!myLumi.begin()) {
        Serial.println("AvantLumi initialization failed! Halting.");
        while (true); // Stop execution if initialization fails
    }
    Serial.println("AvantLumi initialized successfully!");

    // 2. Get Initial Status
    // The getStatus() method returns a JSON-formatted string containing the
    // current state of all major settings (brightness, color, palette, etc.).
    Serial.println("\n2. Retrieving initial status...");
    Serial.println("Initial Status: " + myLumi.getStatus());

    // Set an initial state for the demonstration
    myLumi.setSwitch(true);      // Turn the LEDs on
    myLumi.setBright(3);         // Set to a medium brightness (1-5)
    myLumi.setPalette("ocean");  // Start with a known palette

    Serial.println("\n3. Demonstrating Blend Speed...");
    Serial.println("Blend speed controls how quickly the LEDs transition between palettes/colors.");
    Serial.println("It is a value from 1 (slowest) to 5 (fastest).");
    Serial.println("The demo will now cycle through blend speeds every 5 seconds.");
}

void loop() {
    // CRITICAL: You must call the update() function in your main loop.
    // This function handles all the background work like palette blending,
    // brightness adjustments, and rendering the LED colors.
    myLumi.update();

    // --- Demonstrate Blend Speed and Get Status ---
    // This non-blocking timer will trigger every 5 seconds to change settings.
    // We use EVERY_N_SECONDS, a FastLED macro, for simple, non-blocking timing.
    
    EVERY_N_SECONDS(5) {
        // Get the current blend speed and increment it, cycling from 1 to 5.
        uint8_t currentSpeed = myLumi.getBlendSpeed();
        uint8_t nextSpeed = (currentSpeed % 5) + 1;
        
        Serial.println("\n--------------------");
        Serial.println("Setting blend speed to: " + String(nextSpeed));
        myLumi.setBlendSpeed(nextSpeed);

        // To demonstrate the blend speed, we need to change the palette.
        // This code alternates between the 'forest' and 'lava' palettes.
        // The transition between them will occur at the speed set above.
        if (myLumi.getPalette() == "forest") {
            Serial.println("Blending to 'lava' palette...");
            myLumi.setPalette("lava");
        } else {
            Serial.println("Blending to 'forest' palette...");
            myLumi.setPalette("forest");
        }
        
        // --- Demonstrate Get Status ---
        // After changing the settings, we print the status again to confirm
        // that the blend speed and palette have been updated.
        Serial.println("Current Status: " + myLumi.getStatus());
    }
}
