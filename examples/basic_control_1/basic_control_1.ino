/*
 * AvantLumi - Basic Control Demo (On/Off, Brightness, Fade)
 * 
 * Description:
 * This example demonstrates basic control features of the AvantLumi library, 
 * specifically turning the LEDs on/off, adjusting brightness, and toggling 
 * the fade effect. The demo runs automatically in a loop, showcasing how to
 * use the setSwitch(), setBright(), and setFade() methods.
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
 * It will cycle through a predefined sequence of effects to showcase
 * the library's basic control capabilities. Monitor the serial output at 
 * 115200 baud to see status messages for each step of the demonstration.
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
    TURN_ON,
    BRIGHTNESS_UP,
    BRIGHTNESS_DOWN,
    FADE_OFF,
    FADE_ON,
    TURN_OFF,
    PAUSE_BEFORE_RESTART
};

DemoStep currentStep = TURN_ON;
unsigned long lastStepTime = 0;
int brightness = 1;

void setup() {
    Serial.begin(115200);
    Serial.println("AvantLumi - Basic Control Demo");

    // Initialize the LED controller
    if (myLumi.begin()) {
        Serial.println("AvantLumi initialized successfully!");
    } else {
        Serial.println("AvantLumi initialization failed! Halting.");
        while (true); // Stop execution
    }

    // Set an initial palette so the effects are visible
    // The "party" palette is a good default for vibrant colors.
    myLumi.setPalette("party");
    Serial.println("Initial palette set to 'party'. Starting demo...");
    
    lastStepTime = millis();
}

void loop() {
    // CRITICAL: You must call the update() method in your main loop.
    // This handles all the background work like palette blending,
    // brightness transitions, and updating the LED strip.
    myLumi.update();

    // --- Demonstration Logic ---
    // This section controls the sequence of the demo.
    
    // 1. Demonstrate turning the LEDs ON
    if (currentStep == TURN_ON && millis() - lastStepTime > 2000) {
        Serial.println("\n1. Turning LEDs ON");
        
        // Use setSwitch() to control the on/off state.
        // It accepts a boolean (true/false) or a String ("on"/"off").
        myLumi.setSwitch(true);
        
        lastStepTime = millis();
        currentStep = BRIGHTNESS_UP;
        brightness = 1; // Reset brightness for the next step
    }

    // 2. Demonstrate changing brightness levels (ramping up)
    if (currentStep == BRIGHTNESS_UP && millis() - lastStepTime > 1000) {
        if (brightness <= 5) {
            Serial.println("2. Setting brightness to level: " + String(brightness));
            
            // Use setBright() with a value from 1 to 5.
            myLumi.setBright(brightness);
            
            brightness++;
            lastStepTime = millis();
        } else {
            currentStep = BRIGHTNESS_DOWN;
            brightness = 4; // Prepare for ramp down
        }
    }

    // 3. Demonstrate changing brightness levels (ramping down)
    if (currentStep == BRIGHTNESS_DOWN && millis() - lastStepTime > 1000) {
        if (brightness >= 1) {
            Serial.println("3. Setting brightness to level: " + String(brightness));
            myLumi.setBright(brightness);
            brightness--;
            lastStepTime = millis();
        } else {
            myLumi.setBright(3); // Reset to a medium brightness
            currentStep = FADE_OFF;
        }
    }

    // 4. Demonstrate turning the fade effect OFF
    if (currentStep == FADE_OFF && millis() - lastStepTime > 2000) {
        Serial.println("\n4. Turning Fade Effect OFF");
        
        // Use setFade() to disable the smooth "breathing" effect.
        // The palette colors will be displayed solidly.
        // It accepts a boolean (true/false) or a String ("on"/"off").
        myLumi.setFade(false);
        
        lastStepTime = millis();
        currentStep = FADE_ON;
    }

    // 5. Demonstrate turning the fade effect ON
    if (currentStep == FADE_ON && millis() - lastStepTime > 4000) {
        Serial.println("\n5. Turning Fade Effect ON");
        
        // Re-enable the default fade effect.
        myLumi.setFade(true);
        
        lastStepTime = millis();
        currentStep = TURN_OFF;
    }

    // 6. Demonstrate turning the LEDs OFF
    if (currentStep == TURN_OFF && millis() - lastStepTime > 4000) {
        Serial.println("\n6. Turning LEDs OFF");
        
        // Turn the LEDs off. The library will smoothly fade them out.
        myLumi.setSwitch(false);
        
        lastStepTime = millis();
        currentStep = PAUSE_BEFORE_RESTART;
    }

    // 7. Pause before restarting the demo loop
    if (currentStep == PAUSE_BEFORE_RESTART && millis() - lastStepTime > 3000) {
        Serial.println("\n--- Restarting Demo ---");
        lastStepTime = millis();
        currentStep = TURN_ON;
    }
}
