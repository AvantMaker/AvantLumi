/*
 * AvantLumi Library - Implementation File
 * 
 * By: AvantMaker.com
 * Date: August, 2025
 * 
 * A FastLED-based library for controlling LED strips with color palettes,
 * brightness control, fade effects, and more - without MQTT dependencies.
 * IMPORTANT:Supported DATA pins: 2, 3, 4, 5, 11, 12, 13
 * If you need additional pins, you can easily add more cases to the switch statement. For example, for ESP32 boards, you might want to add cases for pins 16, 17, 18, 19, etc.
 */

#include "AvantLumi.h"

// Static member definitions
const uint8_t AvantLumi::brightnessLevels[6] = {0, 26, 64, 128, 192, 255};

// Custom palette definitions
const CRGBPalette16 AvantLumi::christmas_p = CRGBPalette16(
    CRGB::Red, CRGB::DarkRed, CRGB::Green, CRGB::DarkGreen,
    CRGB::Red, CRGB::Green, CRGB::DarkRed, CRGB::ForestGreen,
    CRGB::Crimson, CRGB::LimeGreen, CRGB::Red, CRGB::Green,
    CRGB::DarkRed, CRGB::SeaGreen, CRGB::Red, CRGB::DarkGreen);

const CRGBPalette16 AvantLumi::autumn_p = CRGBPalette16(
    CRGB::DarkOrange, CRGB::OrangeRed, CRGB::Maroon, CRGB::SaddleBrown,
    CRGB::Orange, CRGB::Chocolate, CRGB::DarkGoldenrod, CRGB::FireBrick,
    CRGB::Peru, CRGB::Sienna, CRGB::DarkOrange, CRGB::Brown,
    CRGB::Goldenrod, CRGB::IndianRed, CRGB::Orange, CRGB::SaddleBrown);

const CRGBPalette16 AvantLumi::cyberpunk_p = CRGBPalette16(
    CRGB::Magenta, CRGB::DeepPink, CRGB::Purple, CRGB::Cyan,
    CRGB::HotPink, CRGB::DarkViolet, CRGB::Aqua, CRGB::Fuchsia,
    CRGB::MediumOrchid, CRGB::Turquoise, CRGB::Violet, CRGB::DeepSkyBlue,
    CRGB::Magenta, CRGB::Purple, CRGB::Cyan, CRGB::HotPink);

const CRGBPalette16 AvantLumi::halloween_p = CRGBPalette16(
    CRGB::Orange, CRGB::DarkOrange, CRGB::Purple, CRGB::Black,
    CRGB::OrangeRed, CRGB::DarkViolet, CRGB::Orange, CRGB::Indigo,
    CRGB::Chocolate, CRGB::Purple, CRGB::DarkOrange, CRGB::MidnightBlue,
    CRGB::Orange, CRGB::DarkSlateBlue, CRGB::SaddleBrown, CRGB::Purple);

const CRGBPalette16 AvantLumi::winter_p = CRGBPalette16(
    CRGB::Blue, CRGB::DarkBlue, CRGB::White, CRGB::LightSteelBlue,
    CRGB::SteelBlue, CRGB::PowderBlue, CRGB::LightBlue, CRGB::Navy,
    CRGB::CornflowerBlue, CRGB::AliceBlue, CRGB::RoyalBlue, CRGB::White,
    CRGB::DodgerBlue, CRGB::LightCyan, CRGB::Blue, CRGB::MidnightBlue);

const CRGBPalette16 AvantLumi::spring_p = CRGBPalette16(
    CRGB::LightGreen, CRGB::Pink, CRGB::LightPink, CRGB::PaleGreen,
    CRGB::SpringGreen, CRGB::Thistle, CRGB::LemonChiffon, CRGB::LightCyan,
    CRGB::MediumSpringGreen, CRGB::Lavender, CRGB::LightYellow, CRGB::Aquamarine,
    CRGB::LightGreen, CRGB::Pink, CRGB::Honeydew, CRGB::PaleTurquoise);

const CRGBPalette16 AvantLumi::sunset_p = CRGBPalette16(
    CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Pink,
    CRGB::Crimson, CRGB::DarkOrange, CRGB::Gold, CRGB::DeepPink,
    CRGB::OrangeRed, CRGB::Coral, CRGB::Orange, CRGB::LightPink,
    CRGB::Red, CRGB::Tomato, CRGB::Yellow, CRGB::HotPink);

const CRGBPalette16 AvantLumi::ocean_deep_p = CRGBPalette16(
    CRGB::DarkBlue, CRGB::MidnightBlue, CRGB::Navy, CRGB::DarkSlateBlue,
    CRGB::Indigo, CRGB::DarkTurquoise, CRGB::SteelBlue, CRGB::DarkCyan,
    CRGB::MediumBlue, CRGB::DarkSeaGreen, CRGB::CadetBlue, CRGB::DarkSlateGray,
    CRGB::DarkBlue, CRGB::Teal, CRGB::Navy, CRGB::MidnightBlue);

const CRGBPalette16 AvantLumi::neon_p = CRGBPalette16(
    CRGB::Lime, CRGB::Cyan, CRGB::Magenta, CRGB::Yellow,
    CRGB::SpringGreen, CRGB::Aqua, CRGB::Fuchsia, CRGB::GreenYellow,
    CRGB::LimeGreen, CRGB::DeepSkyBlue, CRGB::HotPink, CRGB::Gold,
    CRGB::Lime, CRGB::Turquoise, CRGB::Violet, CRGB::Yellow);

const CRGBPalette16 AvantLumi::fire_p = CRGBPalette16(
    CRGB::Red, CRGB::OrangeRed, CRGB::Orange, CRGB::DarkRed,
    CRGB::Crimson, CRGB::DarkOrange, CRGB::Yellow, CRGB::FireBrick,
    CRGB::Red, CRGB::Coral, CRGB::Gold, CRGB::Maroon,
    CRGB::Tomato, CRGB::Orange, CRGB::Red, CRGB::DarkRed);

// Constructor
AvantLumi::AvantLumi(uint8_t dataPin, uint16_t numLeds) {
    this->dataPin = dataPin;
    this->numLeds = numLeds;
    this->leds = new CRGB[numLeds];
    
    // Initialize state variables
    fadeinEnabled = true;
    useRandomPalette = true;
    useSolidColor = false;
    solidColor = CRGB::Red;
    ledEnabled = true;
    currentBrightnessLevel = 3;
    targetBrightness = brightnessLevels[currentBrightnessLevel];
    actualBrightness = brightnessLevels[currentBrightnessLevel];
    lastBrightnessUpdate = 0;
    currentPaletteName = "party";
    solidColorName = "";
    blendSpeed = 4;  // Default to level 4 (fast blending)
    
    // Initialize palettes
    currentPalette = PartyColors_p;
    targetPalette = PartyColors_p;
    currentBlending = LINEARBLEND;

    // Initialize power settings with defaults
    maxVolts = 5;        // Default 5V
    maxMilliamps = 500;  // Default 500mA (same as current hardcoded value)
}

// Destructor
AvantLumi::~AvantLumi() {
    delete[] leds;
}

// Initialization
bool AvantLumi::begin() {
    // Note: FastLED addLeds template requires compile-time constants for pin numbers
    // For dynamic pin assignment, we need a different approach
    // This is a limitation of the FastLED library template system
    
    // For now, we'll use a runtime approach with a switch statement
    // Users can modify this section for their specific pin requirements
    switch(dataPin) {
        case 2:
            FastLED.addLeds<WS2812B, 2, GRB>(leds, numLeds);
            break;
        case 3:
            FastLED.addLeds<WS2812B, 3, GRB>(leds, numLeds);
            break;
        case 4:
            FastLED.addLeds<WS2812B, 4, GRB>(leds, numLeds);
            break;
        case 5:
            FastLED.addLeds<WS2812B, 5, GRB>(leds, numLeds);
            break;
        case 6:
            
            break;
        case 7:
            
            break;
        case 8:
            
            break;
        case 9:
            
            break;
        case 10:
            
            break;
        case 11:
            FastLED.addLeds<WS2812B, 11, GRB>(leds, numLeds);
            break;
        case 12:
            FastLED.addLeds<WS2812B, 12, GRB>(leds, numLeds);
            break;
        case 13:
            FastLED.addLeds<WS2812B, 13, GRB>(leds, numLeds);
            break;
        default:
            // Default to pin 2 if unsupported pin is specified
            FastLED.addLeds<WS2812B, 2, GRB>(leds, numLeds);
            return false; // Return false to indicate unsupported pin
    }
    
    FastLED.setBrightness(brightnessLevels[currentBrightnessLevel]);
    FastLED.setMaxPowerInVoltsAndMilliamps(maxVolts, maxMilliamps);
    return true;
}

// Main update loop
void AvantLumi::update() {
    static unsigned long lastPaletteBlend = 0;
    static unsigned long lastRandomPalette = 0;
    
    updateBrightness();
    
    // Blend palettes
    unsigned long blendInterval;
    uint8_t maxBlendChanges;
    getBlendParameters(blendSpeed, blendInterval, maxBlendChanges);

    if (millis() - lastPaletteBlend >= blendInterval) {
        lastPaletteBlend = millis();
        nblendPaletteTowardPalette(currentPalette, targetPalette, maxBlendChanges);
    }
    
    // Generate random palette
    if (millis() - lastRandomPalette >= 5000) {
        lastRandomPalette = millis();
        if (useRandomPalette && !useSolidColor) {
            generateRandomPalette();
        }
    }
    
    updateLEDs();
    FastLED.show();
}

// Setter methods
bool AvantLumi::setRGB(uint8_t rVal, uint8_t gVal, uint8_t bVal) {
    rVal = constrain(rVal, 0, 255);
    gVal = constrain(gVal, 0, 255);
    bVal = constrain(bVal, 0, 255);
    
    solidColor = CRGB(rVal, gVal, bVal);
    targetPalette = createSolidPalette(solidColor);
    useSolidColor = true;
    useRandomPalette = false;
    currentPaletteName = "solid_color";
    solidColorName = "";
    
    return true;
}

bool AvantLumi::setColor(String colorName) {
    colorName.trim();
    
    if (colorName.length() > 0 && isValidColorName(colorName)) {
        solidColor = parseColorName(colorName);
        solidColorName = colorName;
        targetPalette = createSolidPalette(solidColor);
        useSolidColor = true;
        useRandomPalette = false;
        currentPaletteName = "solid_color";
        return true;
    }
    return false;
}

bool AvantLumi::setBright(uint8_t level) {
    if (level >= 1 && level <= 5) {
        currentBrightnessLevel = level;
        return true;
    }
    return false;
}

bool AvantLumi::setSwitch(String state) {
    state.toLowerCase();
    state.trim();
    
    if (state == "on") {
        ledEnabled = true;
        return true;
    } else if (state == "off") {
        ledEnabled = false;
        return true;
    }
    return false;
}

bool AvantLumi::setSwitch(bool state) {
    ledEnabled = state;
    return true;
}

bool AvantLumi::setFade(String state) {
    state.toLowerCase();
    state.trim();
    
    if (state == "on") {
        fadeinEnabled = true;
        return true;
    } else if (state == "off") {
        fadeinEnabled = false;
        return true;
    }
    return false;
}

bool AvantLumi::setFade(bool state) {
    fadeinEnabled = state;
    return true;
}

bool AvantLumi::setPalette(String paletteName) {
    paletteName.toLowerCase();
    paletteName.trim();
    useSolidColor = false;
    
    if (paletteName == "rainbow") {
        targetPalette = RainbowColors_p;
        useRandomPalette = false;
        currentPaletteName = "rainbow";
    }
    else if (paletteName == "party") {
        targetPalette = PartyColors_p;
        useRandomPalette = false;
        currentPaletteName = "party";
    }
    else if (paletteName == "ocean") {
        targetPalette = OceanColors_p;
        useRandomPalette = false;
        currentPaletteName = "ocean";
    }
    else if (paletteName == "forest") {
        targetPalette = ForestColors_p;
        useRandomPalette = false;
        currentPaletteName = "forest";
    }
    else if (paletteName == "heat") {
        targetPalette = HeatColors_p;
        useRandomPalette = false;
        currentPaletteName = "heat";
    }
    else if (paletteName == "cloud") {
        targetPalette = CloudColors_p;
        useRandomPalette = false;
        currentPaletteName = "cloud";
    }
    else if (paletteName == "lava") {
        targetPalette = LavaColors_p;
        useRandomPalette = false;
        currentPaletteName = "lava";
    }
    else if (paletteName == "u01" || paletteName == "christmas") {
        targetPalette = christmas_p;
        useRandomPalette = false;
        currentPaletteName = "u01_christmas";
    }
    else if (paletteName == "u02" || paletteName == "autumn") {
        targetPalette = autumn_p;
        useRandomPalette = false;
        currentPaletteName = "u02_autumn";
    }
    else if (paletteName == "u03" || paletteName == "cyberpunk") {
        targetPalette = cyberpunk_p;
        useRandomPalette = false;
        currentPaletteName = "u03_cyberpunk";
    }
    else if (paletteName == "u04" || paletteName == "halloween") {
        targetPalette = halloween_p;
        useRandomPalette = false;
        currentPaletteName = "u04_halloween";
    }
    else if (paletteName == "u05" || paletteName == "winter") {
        targetPalette = winter_p;
        useRandomPalette = false;
        currentPaletteName = "u05_winter";
    }
    else if (paletteName == "u06" || paletteName == "spring") {
        targetPalette = spring_p;
        useRandomPalette = false;
        currentPaletteName = "u06_spring";
    }
    else if (paletteName == "u07" || paletteName == "sunset") {
        targetPalette = sunset_p;
        useRandomPalette = false;
        currentPaletteName = "u07_sunset";
    }
    else if (paletteName == "u08" || paletteName == "deep_ocean") {
        targetPalette = ocean_deep_p;
        useRandomPalette = false;
        currentPaletteName = "u08_deep_ocean";
    }
    else if (paletteName == "u09" || paletteName == "neon") {
        targetPalette = neon_p;
        useRandomPalette = false;
        currentPaletteName = "u09_neon";
    }
    else if (paletteName == "u10" || paletteName == "fire") {
        targetPalette = fire_p;
        useRandomPalette = false;
        currentPaletteName = "u10_fire";
    }
    else if (paletteName == "random") {
        useRandomPalette = true;
        currentPaletteName = "random";
    }
    else {
        return false; // Unknown palette
    }
    
    return true;
}

// Getter methods
CRGB AvantLumi::getRGB() {
    return solidColor;
}

String AvantLumi::getColor() {
    return solidColorName;
}

uint8_t AvantLumi::getBright() {
    return currentBrightnessLevel;
}

bool AvantLumi::getSwitch() {
    return ledEnabled;
}

bool AvantLumi::getFade() {
    return fadeinEnabled;
}

String AvantLumi::getPalette() {
    if (useSolidColor) {
        return "solid_color";
    }
    return currentPaletteName;
}

String AvantLumi::getStatus() {
    String status = "{";
    
    // Switch status - changed from "sw" to "switch"
    status += "\"switch\":\"" + String(ledEnabled ? "on" : "off") + "\",";
    
    // Brightness level - changed from "br" to "bright"
    status += "\"bright\":" + String(currentBrightnessLevel) + ",";
    
    // Fade status - changed from "fd" to "fade"
    status += "\"fade\":\"" + String(fadeinEnabled ? "on" : "off") + "\",";
    
    // Color/Palette information
    if (useSolidColor) {
        // Changed from "cr" to "rgb" and "cn" to "color"
        status += "\"rgb\":{";
        status += "\"r\":" + String(solidColor.r) + ",";
        status += "\"g\":" + String(solidColor.g) + ",";
        status += "\"b\":" + String(solidColor.b);
        if (solidColorName.length() > 0) {
            status += ",\"color\":\"" + solidColorName + "\"";
        }
        status += "}";
    } else {
        // Changed from "pal" to "palette"
        status += "\"palette\":\"" + currentPaletteName + "\"";
    }

    // Changed from "pwr" to "power"
    status += ",\"power\":{\"v\":" + String(maxVolts) + ",\"ma\":" + String(maxMilliamps) + "}";
    
    // Keep blend_spd as is since it's not a command name, it's a parameter name
    status += ",\"blend_spd\":" + String(blendSpeed);
    
    status += "}";
    
    return status;
}

// Private helper methods
CRGBPalette16 AvantLumi::createSolidPalette(CRGB color) {
    return CRGBPalette16(color, color, color, color, 
                         color, color, color, color,
                         color, color, color, color,
                         color, color, color, color);
}

void AvantLumi::updateBrightness() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastBrightnessUpdate >= 20) {
        lastBrightnessUpdate = currentTime;
        
        uint8_t desiredBrightness = ledEnabled ? brightnessLevels[currentBrightnessLevel] : 0;
        
        if (targetBrightness != desiredBrightness) {
            targetBrightness = desiredBrightness;
        }
        
        if (actualBrightness < targetBrightness) {
            actualBrightness = min((int)actualBrightness + 3, (int)targetBrightness);
        }
        else if (actualBrightness > targetBrightness) {
            actualBrightness = max((int)actualBrightness - 3, (int)targetBrightness);
        }
        
        FastLED.setBrightness(actualBrightness);
    }
}

void AvantLumi::updateLEDs() {
    random16_set_seed(535);
    
    for (int i = 0; i < numLeds; i++) {
        uint8_t fader;
        
        if (fadeinEnabled) {
            fader = sin8(millis() / random8(10, 20));
        } else {
            fader = 255;
        }
        
        leds[i] = ColorFromPalette(currentPalette, i * 20, fader, currentBlending);
    }
    
    random16_set_seed(millis());
}

void AvantLumi::generateRandomPalette() {
    uint8_t baseC = random8(255);
    targetPalette = CRGBPalette16(CHSV(baseC + random8(0, 32), 255, random8(128, 255)), 
                                  CHSV(baseC + random8(0, 32), 255, random8(128, 255)), 
                                  CHSV(baseC + random8(0, 32), 192, random8(128, 255)), 
                                  CHSV(baseC + random8(0, 32), 255, random8(128, 255)));
}

CRGB AvantLumi::parseColorName(String colorName) {
    colorName.toLowerCase();
    
    // Basic colors
    if (colorName == "red") return CRGB::Red;
    else if (colorName == "green") return CRGB::Green;
    else if (colorName == "blue") return CRGB::Blue;
    else if (colorName == "white") return CRGB::White;
    else if (colorName == "black") return CRGB::Black;
    else if (colorName == "yellow") return CRGB::Yellow;
    else if (colorName == "cyan") return CRGB::Cyan;
    else if (colorName == "magenta") return CRGB::Magenta;
    else if (colorName == "orange") return CRGB::Orange;
    else if (colorName == "purple") return CRGB::Purple;
    else if (colorName == "pink") return CRGB::Pink;
    else if (colorName == "brown") return CRGB::Brown;
    
    // Extended colors - Light variations
    else if (colorName == "lightgreen") return CRGB::LightGreen;
    else if (colorName == "lightblue") return CRGB::LightBlue;
    else if (colorName == "lightpink") return CRGB::LightPink;
    else if (colorName == "lightcyan") return CRGB::LightCyan;
    else if (colorName == "lightyellow") return CRGB::LightYellow;
    else if (colorName == "lightsteelblue") return CRGB::LightSteelBlue;
    
    // Extended colors - Dark variations
    else if (colorName == "darkred") return CRGB::DarkRed;
    else if (colorName == "darkgreen") return CRGB::DarkGreen;
    else if (colorName == "darkblue") return CRGB::DarkBlue;
    else if (colorName == "darkorange") return CRGB::DarkOrange;
    else if (colorName == "darkviolet") return CRGB::DarkViolet;
    else if (colorName == "darkgray" || colorName == "darkgrey") return CRGB::DarkGray;
    else if (colorName == "darkcyan") return CRGB::DarkCyan;
    else if (colorName == "darkgoldenrod") return CRGB::DarkGoldenrod;
    else if (colorName == "darkslateblue") return CRGB::DarkSlateBlue;
    else if (colorName == "darkturquoise") return CRGB::DarkTurquoise;
    else if (colorName == "darkseagreen") return CRGB::DarkSeaGreen;
    
    // Extended colors - Medium variations
    else if (colorName == "mediumblue") return CRGB::MediumBlue;
    else if (colorName == "mediumorchid") return CRGB::MediumOrchid;
    else if (colorName == "mediumspringgreen") return CRGB::MediumSpringGreen;
    
    // Special colors
    else if (colorName == "springgreen") return CRGB::SpringGreen;
    else if (colorName == "forestgreen") return CRGB::ForestGreen;
    else if (colorName == "seagreen") return CRGB::SeaGreen;
    else if (colorName == "limegreen") return CRGB::LimeGreen;
    else if (colorName == "lime") return CRGB::Lime;
    else if (colorName == "aqua") return CRGB::Aqua;
    else if (colorName == "aquamarine") return CRGB::Aquamarine;
    else if (colorName == "turquoise") return CRGB::Turquoise;
    else if (colorName == "palegreen") return CRGB::PaleGreen;
    else if (colorName == "paleturquoise") return CRGB::PaleTurquoise;
    else if (colorName == "powderblue") return CRGB::PowderBlue;
    else if (colorName == "steelblue") return CRGB::SteelBlue;
    else if (colorName == "royalblue") return CRGB::RoyalBlue;
    else if (colorName == "cornflowerblue") return CRGB::CornflowerBlue;
    else if (colorName == "deepskyblue") return CRGB::DeepSkyBlue;
    else if (colorName == "dodgerblue") return CRGB::DodgerBlue;
    else if (colorName == "midnightblue") return CRGB::MidnightBlue;
    else if (colorName == "navy") return CRGB::Navy;
    else if (colorName == "indigo") return CRGB::Indigo;
    else if (colorName == "violet") return CRGB::Violet;
    else if (colorName == "fuchsia") return CRGB::Fuchsia;
    else if (colorName == "hotpink") return CRGB::HotPink;
    else if (colorName == "deeppink") return CRGB::DeepPink;
    else if (colorName == "crimson") return CRGB::Crimson;
    else if (colorName == "firebrick") return CRGB::FireBrick;
    else if (colorName == "maroon") return CRGB::Maroon;
    else if (colorName == "orangered") return CRGB::OrangeRed;
    else if (colorName == "tomato") return CRGB::Tomato;
    else if (colorName == "coral") return CRGB::Coral;
    else if (colorName == "chocolate") return CRGB::Chocolate;
    else if (colorName == "saddlebrown") return CRGB::SaddleBrown;
    else if (colorName == "sienna") return CRGB::Sienna;
    else if (colorName == "peru") return CRGB::Peru;
    else if (colorName == "goldenrod") return CRGB::Goldenrod;
    else if (colorName == "gold") return CRGB::Gold;
    else if (colorName == "greenyellow") return CRGB::GreenYellow;
    else if (colorName == "lemonchiffon") return CRGB::LemonChiffon;
    else if (colorName == "honeydew") return CRGB::Honeydew;
    else if (colorName == "lavender") return CRGB::Lavender;
    else if (colorName == "thistle") return CRGB::Thistle;
    else if (colorName == "aliceblue") return CRGB::AliceBlue;
    else if (colorName == "gray" || colorName == "grey") return CRGB::Gray;
    else if (colorName == "silver") return CRGB::Silver;
    else if (colorName == "teal") return CRGB::Teal;
    else if (colorName == "cadetblue") return CRGB::CadetBlue;
    else if (colorName == "darkslategray" || colorName == "darkslategrey") return CRGB::DarkSlateGray;
    else if (colorName == "indianred") return CRGB::IndianRed;
    
    // If color name not found, return Red as default
    return CRGB::Red;
}

bool AvantLumi::isValidColorName(String colorName) {
    colorName.toLowerCase();
    
    // List of all supported color names
    String validColors[] = {
        "red", "green", "blue", "white", "black", "yellow", "cyan", "magenta", "orange", "purple", "pink", "brown",
        "lightgreen", "lightblue", "lightpink", "lightcyan", "lightyellow", "lightsteelblue",
        "darkred", "darkgreen", "darkblue", "darkorange", "darkviolet", "darkgray", "darkgrey", "darkcyan", 
        "darkgoldenrod", "darkslateblue", "darkturquoise", "darkseagreen",
        "mediumblue", "mediumorchid", "mediumspringgreen",
        "springgreen", "forestgreen", "seagreen", "limegreen", "lime", "aqua", "aquamarine", "turquoise",
        "palegreen", "paleturquoise", "powderblue", "steelblue", "royalblue", "cornflowerblue", "deepskyblue",
        "dodgerblue", "midnightblue", "navy", "indigo", "violet", "fuchsia", "hotpink", "deeppink", "crimson",
        "firebrick", "maroon", "orangered", "tomato", "coral", "chocolate", "saddlebrown", "sienna", "peru",
        "goldenrod", "gold", "greenyellow", "lemonchiffon", "honeydew", "lavender", "thistle", "aliceblue",
        "gray", "grey", "silver", "teal", "cadetblue", "darkslategray", "darkslategrey", "indianred"
    };
    
    int numColors = sizeof(validColors) / sizeof(validColors[0]);
    
    for (int i = 0; i < numColors; i++) {
        if (colorName == validColors[i]) {
            return true;
        }
    }
    return false;
}

bool AvantLumi::setMaxPower(uint8_t voltsVal, uint32_t milliamps) {
    // Validate voltage (common values: 3, 5, 12, 24V)
    if (voltsVal < 3 || voltsVal > 24) {
        return false;
    }
    
    // Validate current (reasonable range: 50mA to 20A)
    if (milliamps < 50 || milliamps > 20000) {
        return false;
    }
    
    maxVolts = voltsVal;
    maxMilliamps = milliamps;
    
    // Apply the new power settings to FastLED
    FastLED.setMaxPowerInVoltsAndMilliamps(maxVolts, maxMilliamps);
    
    return true;
}

uint8_t AvantLumi::getMaxVolts() {
    return maxVolts;
}

uint32_t AvantLumi::getMaxMilliamps() {
    return maxMilliamps;
}

bool AvantLumi::setBlendSpeed(uint8_t speed_val) {
    if (speed_val >= 1 && speed_val <= 5) {
        blendSpeed = speed_val;
        return true;
    }
    return false;
}

uint8_t AvantLumi::getBlendSpeed() {
    return blendSpeed;
}

void AvantLumi::getBlendParameters(uint8_t speedLevel, unsigned long& interval, uint8_t& maxChanges) {
    switch(speedLevel) {
        case 1:  // Slowest
            interval = 200;   // 200ms
            maxChanges = 25;
            break;
        case 2:  // Current speed (default)
            interval = 100;   // 100ms  
            maxChanges = 50;
            break;
        case 3:  // Faster
            interval = 50;    // 50ms
            maxChanges = 75;
            break;
        case 4:  // Fast
            interval = 25;    // 25ms
            maxChanges = 100;
            break;
        case 5:  // Fastest
            interval = 10;    // 10ms
            maxChanges = 150;
            break;
        default:
            interval = 100;   // Default to level 2
            maxChanges = 50;
            break;
    }
}

const uint32_t CONFIG_MAGIC = 0x4C554D49; // "LUMI"

// Structure to hold configuration data for EEPROM
struct LedConfig {
    uint32_t magic;
    bool ledEnabled;
    uint8_t currentBrightnessLevel;
    bool fadeinEnabled;
    bool useSolidColor;
    CRGB solidColor;
    char currentPaletteName[32];
    char solidColorName[32];
    bool useRandomPalette;
    uint8_t blendSpeed;  // Add this line
};

bool AvantLumi::saveConfig() {
    LedConfig config;
    config.magic = CONFIG_MAGIC;
    config.ledEnabled = this->ledEnabled;
    config.currentBrightnessLevel = this->currentBrightnessLevel;
    config.fadeinEnabled = this->fadeinEnabled;
    config.useSolidColor = this->useSolidColor;
    config.solidColor = this->solidColor;
    strncpy(config.currentPaletteName, this->currentPaletteName.c_str(), sizeof(config.currentPaletteName) - 1);
    config.currentPaletteName[sizeof(config.currentPaletteName) - 1] = '\0';
    strncpy(config.solidColorName, this->solidColorName.c_str(), sizeof(config.solidColorName) - 1);
    config.solidColorName[sizeof(config.solidColorName) - 1] = '\0';
    config.useRandomPalette = this->useRandomPalette;
    config.blendSpeed = this->blendSpeed;

    EEPROM.begin(sizeof(LedConfig));
    EEPROM.put(0, config);
    return EEPROM.commit();
}

bool AvantLumi::loadConfig() {
    LedConfig config;
    EEPROM.begin(sizeof(LedConfig));
    EEPROM.get(0, config);

    if (config.magic != CONFIG_MAGIC) {
        return false; // No valid config found
    }

    this->ledEnabled = config.ledEnabled;
    this->currentBrightnessLevel = config.currentBrightnessLevel;
    this->fadeinEnabled = config.fadeinEnabled;
    this->useSolidColor = config.useSolidColor;
    this->solidColor = config.solidColor;
    this->currentPaletteName = String(config.currentPaletteName);
    this->solidColorName = String(config.solidColorName);
    this->useRandomPalette = config.useRandomPalette;
    this->blendSpeed = config.blendSpeed;

    // Restore palette state
    if (this->useSolidColor) {
        this->targetPalette = createSolidPalette(this->solidColor);
    } else {
        setPalette(this->currentPaletteName);
    }

    // Update brightness
    this->targetBrightness = brightnessLevels[this->currentBrightnessLevel];
    
    return true;
}

bool AvantLumi::checkConfig() {
    EEPROM.begin(sizeof(LedConfig));
    uint32_t magic;
    EEPROM.get(0, magic);
    return magic == CONFIG_MAGIC;
}