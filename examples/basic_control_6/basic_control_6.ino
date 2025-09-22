/*
 * AvantLumi - EEPROM Configuration Handling Demo
 *
 * Description:
 * This example demonstrates how to use the EEPROM features of the AvantLumi library
 * to save, load, and check for LED configurations. The sketch checks for a saved
 * configuration in EEPROM on startup. If none is found, it sets a default configuration
 * (blue color, brightness level 4) and saves it. If a configuration is found, it loads
 * and applies the settings. You can change the configuration (e.g., set a new color or palette),
 * and the new settings will be saved automatically. Reset your ESP32 to see the saved
 * configuration being loaded on startup.
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
 * 1. Upload this sketch to your ESP32.
 * 2. Open the Serial Monitor at 115200 baud.
 * 3. The sketch will first check if a configuration is saved in EEPROM.
 * 4. If no configuration is found, it will set a default configuration (blue color, brightness level 4) and save it.
 * 5. If a configuration is found, it will load it and apply the settings.
 * 6. You can then change the configuration (e.g., set a new color or palette) and the new settings will be saved automatically.
 * 7. Reset your ESP32 to see the saved configuration being loaded on startup.
 *
 * Compatibility: Tested with ESP32 DevKitC and DOIT ESP32 DevKit boards.
 */

#include <AvantLumi.h>

// LED strip configuration
#define LED_PIN 2
#define NUM_LEDS 30

// Create an AvantLumi instance
AvantLumi leds(LED_PIN, NUM_LEDS);

void setup() {
    Serial.begin(115200);
    delay(1000); // Wait for serial to initialize

    // Initialize the AvantLumi library
    if (!leds.begin()) {
        Serial.println("Failed to initialize AvantLumi (unsupported pin?)");
        while (true);
    }

    // Check if a configuration exists in EEPROM
    if (leds.checkConfig()) {
        Serial.println("Configuration found in EEPROM. Loading...");
        if (leds.loadConfig()) {
            Serial.println("Configuration loaded successfully.");
        } else {
            Serial.println("Failed to load configuration. Using defaults.");
            setDefaultConfig();
        }
    } else {
        Serial.println("No configuration found in EEPROM. Saving default configuration.");
        setDefaultConfig();
        if (leds.saveConfig()) {
            Serial.println("Default configuration saved.");
        } else {
            Serial.println("Failed to save default configuration.");
        }
    }

    Serial.println("Initial configuration:");
    Serial.println(leds.getStatus());
}

void loop() {
    // Main update loop for the LEDs
    leds.update();

    // Example of changing the configuration after some time
    // This will be saved automatically if you call saveConfig()
    if (millis() > 15000 && millis() < 15100) {
        Serial.println("\nChanging palette to 'ocean' and brightness to 2...");
        leds.setPalette("ocean");
        leds.setBright(2);
        
        // Save the new configuration
        if (leds.saveConfig()) {
            Serial.println("New configuration saved.");
            Serial.println("Reset the board to see the loaded configuration.");
        } else {
            Serial.println("Failed to save new configuration.");
        }
        Serial.println(leds.getStatus());
    }
}

// Helper function to set a default configuration
void setDefaultConfig() {
    leds.setSwitch(true);
    leds.setColor("blue");
    leds.setBright(4);
    leds.setFade(true);
}
