/*
 * AvantLumi - Basic Demo
 * 
 * Description:
 * This example demonstrates the core features of the AvantLumi library
 * by cycling through various LED effects automatically. It showcases
 * color settings, brightness control, fade effects, and palette
 * transitions in a continuous loop.
 * 
 * Author: AvantMaker <admin@avantmaker.com>
 * Author Website: https://www.AvantMaker.com
 * Date: August 31, 2025
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
 * It will cycle through a predefined sequence of LED effects to showcase
 * the library's capabilities. Monitor the serial output at 115200 baud
 * to see status messages for each step of the demonstration.
 * 
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

#include "AvantLumi.h"

// LED Configuration
#define DATA_PIN 2
#define NUM_LEDS 30

// Create AvantLumi object
AvantLumi myLumi(DATA_PIN, NUM_LEDS);

// Demo state variables
enum DemoState {
    DEMO_IDLE,
    DEMO_RGB_RED,
    DEMO_COLOR_BLUE,
    DEMO_BRIGHTNESS,
    DEMO_SWITCH_OFF,
    DEMO_SWITCH_ON,
    DEMO_FADE_OFF,
    DEMO_FADE_ON,
    DEMO_BUILTIN_PALETTES,
    DEMO_CUSTOM_PALETTES,
    DEMO_RANDOM_PALETTE,
    DEMO_GETTERS,
    DEMO_FINAL_STATUS
};

DemoState currentDemoState = DEMO_IDLE;
unsigned long demoStartTime = 0;
int demoStep = 0;
int brightnessLevel = 1;
int paletteIndex = 0;
int customPaletteIndex = 0;

// Built-in palette names
String builtinPalettes[] = {"rainbow", "party", "ocean", "forest", "heat", "cloud", "lava"};
int numBuiltinPalettes = sizeof(builtinPalettes) / sizeof(builtinPalettes[0]);

// Custom palette names
String customPalettes[] = {"christmas", "autumn", "cyberpunk", "halloween", "winter", "spring", "sunset", "neon", "fire"};
int numCustomPalettes = sizeof(customPalettes) / sizeof(customPalettes[0]);

void setup() {
    Serial.begin(115200);
    Serial.println("AvantLumi Library Basic Demo Starting...");

    // Set power limits (e.g., 5V, 500mA)
    if (myLumi.setMaxPower(5, 500)) {
        Serial.println("Power limit set to 5V, 500mA");
    } else {
        Serial.println("Invalid power settings");
    }

    // Initialize the LED controller
    if (myLumi.begin()) {
        Serial.println("AvantLumi initialized successfully!");
    } else {
        Serial.println("AvantLumi initialization failed!");
        return;
    }

    // Print initial status
    Serial.println("Initial Status: " + myLumi.getStatus());
    
    // Start the demonstration
    Serial.println("\n=== Starting AvantLumi Features Demo ===");
    startNextDemo();
}

void loop() {
    // CRITICAL: Call update() continuously to keep LEDs responsive
    myLumi.update();
    
    // Run demonstration without blocking
    runDemo();
}

void startNextDemo() {
    demoStartTime = millis();
    demoStep = 0;
    
    switch(currentDemoState) {
        case DEMO_IDLE:
            currentDemoState = DEMO_RGB_RED;
            Serial.println("1. Setting RGB color to red (255, 0, 0)");
            myLumi.setRGB(255, 0, 0);
            break;
            
        case DEMO_RGB_RED:
            currentDemoState = DEMO_COLOR_BLUE;
            Serial.println("Status: " + myLumi.getStatus());
            Serial.println("\n2. Setting color to 'blue'");
            myLumi.setColor("blue");
            break;
            
        case DEMO_COLOR_BLUE:
            currentDemoState = DEMO_BRIGHTNESS;
            Serial.println("Current color: " + myLumi.getColor());
            Serial.println("\n3. Testing brightness levels");
            brightnessLevel = 1;
            break;
            
        case DEMO_BRIGHTNESS:
            currentDemoState = DEMO_SWITCH_OFF;
            Serial.println("\n4. Testing switch on/off");
            myLumi.setSwitch("off");
            Serial.println("LEDs turned OFF");
            break;
            
        case DEMO_SWITCH_OFF:
            currentDemoState = DEMO_SWITCH_ON;
            myLumi.setSwitch(true);  // Using boolean
            Serial.println("LEDs turned ON");
            break;
            
        case DEMO_SWITCH_ON:
            currentDemoState = DEMO_FADE_OFF;
            Serial.println("\n5. Testing fade effects");
            myLumi.setFade(false);
            Serial.println("Fade effects disabled");
            break;
            
        case DEMO_FADE_OFF:
            currentDemoState = DEMO_FADE_ON;
            myLumi.setFade("on");
            Serial.println("Fade effects enabled");
            break;
            
        case DEMO_FADE_ON:
            currentDemoState = DEMO_BUILTIN_PALETTES;
            Serial.println("\n6. Testing built-in palettes");
            paletteIndex = 0;
            break;
            
        case DEMO_BUILTIN_PALETTES:
            currentDemoState = DEMO_CUSTOM_PALETTES;
            Serial.println("\n7. Testing custom themed palettes");
            customPaletteIndex = 0;
            break;
            
        case DEMO_CUSTOM_PALETTES:
            currentDemoState = DEMO_RANDOM_PALETTE;
            Serial.println("\n8. Setting random palette");
            myLumi.setPalette("random");
            Serial.println("Random palette enabled - colors will change automatically");
            break;
            
        case DEMO_RANDOM_PALETTE:
            currentDemoState = DEMO_GETTERS;
            Serial.println("\n9. Demonstrating getter methods");
            break;
            
        case DEMO_GETTERS:
            currentDemoState = DEMO_FINAL_STATUS;
            // Set to a solid color to test RGB getter
            myLumi.setColor("hotpink");
            Serial.println("\n10. Final status report:");
            break;
            
        case DEMO_FINAL_STATUS:
            currentDemoState = DEMO_IDLE;
            Serial.println("\n=== Demo Complete, Restarting... ===");
            startNextDemo();
            break;
    }
}

void runDemo() {
    // Use non-blocking delays to cycle through demo steps
    EVERY_N_MILLISECONDS(2000) {
        switch(currentDemoState) {
            case DEMO_RGB_RED:
            case DEMO_COLOR_BLUE:
            case DEMO_SWITCH_OFF:
            case DEMO_SWITCH_ON:
            case DEMO_FADE_OFF:
            case DEMO_FADE_ON:
                startNextDemo();
                break;
                
            case DEMO_RANDOM_PALETTE:
                if (millis() - demoStartTime >= 5000) {
                    startNextDemo();
                }
                break;
                
            default:
                // Other states handled by different timings
                break;
        }
    }
    
    // Handle brightness level cycling every 1 second
    EVERY_N_MILLISECONDS(1000) {
        if (currentDemoState == DEMO_BRIGHTNESS) {
            if (brightnessLevel <= 5) {
                myLumi.setBright(brightnessLevel);
                Serial.println("Brightness level: " + String(brightnessLevel));
                brightnessLevel++;
            } else {
                startNextDemo();
            }
        }
    }
    
    // Handle built-in palette cycling every 3 seconds
    EVERY_N_MILLISECONDS(3000) {
        if (currentDemoState == DEMO_BUILTIN_PALETTES) {
            if (paletteIndex < numBuiltinPalettes) {
                myLumi.setPalette(builtinPalettes[paletteIndex]);
                Serial.println("Palette set to: " + builtinPalettes[paletteIndex]);
                paletteIndex++;
            } else {
                startNextDemo();
            }
        }
        
        if (currentDemoState == DEMO_CUSTOM_PALETTES) {
            if (customPaletteIndex < numCustomPalettes) {
                myLumi.setPalette(customPalettes[customPaletteIndex]);
                Serial.println("Custom palette set to: " + customPalettes[customPaletteIndex]);
                customPaletteIndex++;
            } else {
                startNextDemo();
            }
        }
    }
    
    // Handle getter methods demonstration
    EVERY_N_MILLISECONDS(1000) {
        if (currentDemoState == DEMO_GETTERS && demoStep == 0) {
            Serial.println("Current brightness level: " + String(myLumi.getBright()));
            Serial.println("Switch state: " + String(myLumi.getSwitch() ? "ON" : "OFF"));
            Serial.println("Fade enabled: " + String(myLumi.getFade() ? "YES" : "NO"));
            Serial.println("Current palette: " + myLumi.getPalette());
            demoStep = 1;
        }
    }

    EVERY_N_MILLISECONDS(2000) {
        if (currentDemoState == DEMO_GETTERS && demoStep == 1) {
            startNextDemo();
        }
    }

    // Handle final status display
    EVERY_N_MILLISECONDS(1000) {
        if (currentDemoState == DEMO_FINAL_STATUS && demoStep == 0) {
            CRGB currentRGB = myLumi.getRGB();
            Serial.println("Current RGB: R=" + String(currentRGB.r) + 
                           ", G=" + String(currentRGB.g) + 
                           ", B=" + String(currentRGB.b));
            Serial.println(myLumi.getStatus());
            demoStep = 1;
        }
    }

    EVERY_N_MILLISECONDS(3000) {
        if (currentDemoState == DEMO_FINAL_STATUS && demoStep == 1) {
            startNextDemo();
        }
    }
}
