/*
 * AvantLumi Library - Header File
 * 
 * By: AvantMaker.com
 * Date: August, 2025
 * 
 * A FastLED-based library for controlling LED strips with color palettes,
 * brightness control, fade effects, and more - without MQTT dependencies.
 * 
 * Based on the FastLED_MQTT_Controller reference implementation.
 */

#ifndef AVANTLUMI_H
#define AVANTLUMI_H

#include "FastLED.h"
#include <EEPROM.h>

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

class AvantLumi {
private:
    // LED configuration
    CRGB* leds;
    uint8_t dataPin;
    uint16_t numLeds;
    
    // State variables
    bool fadeinEnabled;
    bool useRandomPalette;
    bool useSolidColor;
    CRGB solidColor;
    bool ledEnabled;
    uint8_t currentBrightnessLevel;
    uint8_t targetBrightness;
    uint8_t actualBrightness;
    unsigned long lastBrightnessUpdate;
    String currentPaletteName;
    String solidColorName;
    
    // Brightness levels (0-5)
    static const uint8_t brightnessLevels[6];
    uint8_t blendSpeed;  // 1-5 scale, 1=slowest, 5=fastest
    
    // Palette variables
    CRGBPalette16 currentPalette;
    CRGBPalette16 targetPalette;
    TBlendType currentBlending;
    
    // Custom palette definitions
    static const CRGBPalette16 christmas_p;
    static const CRGBPalette16 autumn_p;
    static const CRGBPalette16 cyberpunk_p;
    static const CRGBPalette16 halloween_p;
    static const CRGBPalette16 winter_p;
    static const CRGBPalette16 spring_p;
    static const CRGBPalette16 sunset_p;
    static const CRGBPalette16 ocean_deep_p;
    static const CRGBPalette16 neon_p;
    static const CRGBPalette16 fire_p;
    
    // Private helper methods
    CRGBPalette16 createSolidPalette(CRGB color);
    void updateBrightness();
    void updateLEDs();
    CRGB parseColorName(String colorName);
    bool isValidColorName(String colorName);
    void generateRandomPalette();
    void getBlendParameters(uint8_t speedLevel, unsigned long& interval, uint8_t& maxChanges);

    uint8_t maxVolts;
    uint32_t maxMilliamps;

public:
    // Constructor
    AvantLumi(uint8_t dataPin, uint16_t numLeds);
    
    // Destructor
    ~AvantLumi();
    
    // Initialization
    bool begin();
    
    // Main update loop (call this in Arduino loop())
    void update();
    
    // Setter methods
    bool setRGB(uint8_t rVal, uint8_t gVal, uint8_t bVal);
    bool setColor(String colorName);
    bool setBright(uint8_t level);
    bool setSwitch(String state);
    bool setSwitch(bool state);
    bool setFade(String state);
    bool setFade(bool state);
    bool setPalette(String paletteName);
    bool setBlendSpeed(uint8_t speed_val);
    
    // Getter methods
    CRGB getRGB();
    String getColor();
    uint8_t getBright();
    bool getSwitch();
    bool getFade();
    String getPalette();
    String getStatus();
    uint8_t getBlendSpeed();

    bool setMaxPower(uint8_t voltsVal, uint32_t milliamps);
    uint8_t getMaxVolts();
    uint32_t getMaxMilliamps();    

    // EEPROM configuration
    bool saveConfig();
    bool loadConfig();
    bool checkConfig();
};

#endif // AVANTLUMI_H