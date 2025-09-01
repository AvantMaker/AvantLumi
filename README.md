# AvantLumi Library

**README Version 1.0.1** • **Revised: September 1, 2025** • **Author: AvantMaker** • **https://www.AvantMaker.com**

*This project is proudly brought to you by the team at AvantMaker.com.*

Visit us at [AvantMaker.com](https://www.avantmaker.com) where we've crafted a comprehensive collection of Reference and Tutorial materials for the ESP32, Arduino, and other microcontrollers that power countless DIY and IoT creations.

---

![AvantMaker AvantLumi - Get Ready to Shine!!](https://avantmaker.com/wp-content/uploads/2025/09/avantlumi_theme_Image.jpg)
## 🌟 Overview

**AvantLumi** is a powerful Arduino library built on top of FastLED that provides an intuitive interface for controlling LED strips with advanced features including:

- **Color Palettes**: Built-in and custom palettes for stunning visual effects
- **Brightness Control**: 5-level brightness control with smooth transitions
- **Fade Effects**: Configurable fade-in animations
- **Solid Colors**: Support for over 80 named colors and RGB values
- **Power Management**: Configurable voltage and current limits
- **EEPROM Storage**: Save and restore LED configurations
- **Easy Integration**: Simple API for quick setup and control

Perfect for LED projects, ambient lighting, and any application requiring sophisticated LED control.

---

## ✨ Key Features

### 🎨 **Color Control**
- **RGB Values**: Set colors using RGB values (0-255)
- **Named Colors**: Use intuitive color names like "red", "blue", "springgreen"
- **80+ Colors**: Extensive color library including variations like "lightblue", "darkgreen"

### 🌈 **Palette System**
- **Built-in Palettes**: rainbow, party, ocean, forest, heat, cloud, lava
- **Seasonal Palettes**: christmas, autumn, winter, spring, halloween
- **Themed Palettes**: cyberpunk, sunset, deep_ocean, neon, fire
- **Random Generation**: Dynamic palette generation for variety
- **Smooth Blending**: Configurable blend speeds (5 levels)

### 💡 **Brightness & Effects**
- **5 Brightness Levels**: From dim ambient lighting to full brightness
- **Smooth Transitions**: Fade between brightness levels
- **Fade Effects**: Toggle fade-in animations on/off
- **Power Limits**: Configure voltage and current limits for safety

### 💾 **Configuration Management**
- **EEPROM Storage**: Automatically save settings
- **Persistent State**: Restore last configuration on power-up
- **JSON Status**: Get complete system status in JSON format

---

## 📋 Requirements

### Hardware Requirements
- **ESP32 or compatible microcontroller**
- **WS2812B LED strip** (or compatible addressable LEDs)
- **Supported Data Pins**: 2, 3, 4, 5, 11, 12, 13 (easily expandable)

### Software Dependencies
- **FastLED Library** (version 3.1.0 or higher)

> **Note**: FastLED version 3.1.0 or later is required. Check [FastLED GitHub](https://github.com/FastLED/FastLED) for the latest version.

---

## 🚀 Quick Start

### Installation

1. **Download** the latest release from GitHub
2. **In Arduino IDE**:
   - Sketch → Include Library → Add .ZIP Library...
   - Select the downloaded ZIP file
3. **Install Dependencies**:
   - Install FastLED library from Library Manager

Alternatively, clone into your Arduino libraries folder:
```bash
git clone https://github.com/AvantMaker/AvantLumi.git
```

### Basic Usage

```cpp
/*
  AvantLumi Basic Example
  Controls a WS2812B LED strip with color palettes and effects
*/

#include "AvantLumi.h"

// Configuration
#define DATA_PIN 2        // LED data pin
#define NUM_LEDS 60       // Number of LEDs in strip

// Initialize AvantLumi
AvantLumi lumi(DATA_PIN, NUM_LEDS);

void setup() {
  Serial.begin(115200);
  
  // Initialize the LED controller
  if (!lumi.begin()) {
    Serial.println("Warning: Unsupported pin, using pin 2 as default");
  }
  
  // Optional: Load saved configuration from EEPROM
  if (lumi.checkConfig()) {
    lumi.loadConfig();
    Serial.println("Configuration loaded from EEPROM");
  }
  
  // Set initial configuration
  lumi.setSwitch(true);           // Turn on LEDs
  lumi.setBright(3);              // Set brightness level (1-5)
  lumi.setPalette("rainbow");     // Set rainbow palette
  lumi.setFade(true);             // Enable fade effects
  
  Serial.println("AvantLumi initialized!");
}

void loop() {
  // Update LEDs (call this regularly)
  lumi.update();
  
  // Optional: Save configuration periodically
  // lumi.saveConfig();
}
```

---

## 🎨 API Reference

### Initialization

```cpp
AvantLumi(uint8_t dataPin, uint16_t numLeds)
```
- **dataPin**: GPIO pin connected to LED data line
- **numLeds**: Number of LEDs in the strip

```cpp
bool begin()
```
Initialize the LED controller. Returns `false` if pin is unsupported.

### Color Control

```cpp
bool setRGB(uint8_t r, uint8_t g, uint8_t b)
```
Set solid color using RGB values (0-255).

```cpp
bool setColor(String colorName)
```
Set color using name (e.g., "red", "blue", "springgreen").

**Supported Colors**: red, green, blue, white, black, yellow, cyan, magenta, orange, purple, pink, brown, lightgreen, lightblue, lightpink, darkred, darkgreen, darkblue, springgreen, forestgreen, limegreen, hotpink, crimson, navy, gold, silver, and many more!

### Palette Control

```cpp
bool setPalette(String paletteName)
```

**Built-in Palettes**:
- `"rainbow"` - Classic rainbow colors
- `"party"` - Vibrant party colors  
- `"ocean"` - Ocean blue tones
- `"forest"` - Forest green tones
- `"heat"` - Heat/fire colors
- `"cloud"` - Soft cloud colors
- `"lava"` - Lava red/orange tones

**Seasonal Palettes**:
- `"christmas"` or `"u01"` - Red and green holiday colors
- `"autumn"` or `"u02"` - Orange and brown autumn tones  
- `"winter"` or `"u05"` - Blue and white winter colors
- `"spring"` or `"u06"` - Light green and pink spring colors
- `"halloween"` or `"u04"` - Orange and purple spooky colors

**Themed Palettes**:
- `"cyberpunk"` or `"u03"` - Neon pink and cyan
- `"sunset"` or `"u07"` - Warm sunset colors
- `"deep_ocean"` or `"u08"` - Deep sea blues
- `"neon"` or `"u09"` - Bright neon colors
- `"fire"` or `"u10"` - Fire red and orange
- `"random"` - Dynamically generated random palettes

### Brightness & Effects

```cpp
bool setBright(uint8_t level)        // Set brightness (1-5)
bool setSwitch(bool state)           // Turn LEDs on/off
bool setFade(bool state)             // Enable/disable fade effects
bool setBlendSpeed(uint8_t speed)    // Set palette blend speed (1-5)
```

### Power Management

```cpp
bool setMaxPower(uint8_t volts, uint32_t milliamps)
```
Configure power limits for LED safety.
- **volts**: Supply voltage (3-24V)
- **milliamps**: Current limit (50-20000mA)

### Configuration Management

```cpp
bool saveConfig()      // Save current settings to EEPROM
bool loadConfig()      // Load settings from EEPROM  
bool checkConfig()     // Check if valid config exists
```

### Status & Information

```cpp
String getStatus()     // Get complete status as JSON
CRGB getRGB()         // Get current RGB values
String getColor()     // Get current color name
uint8_t getBright()   // Get brightness level
bool getSwitch()      // Get on/off state
String getPalette()   // Get current palette name
```

---

## 📊 Supported Data Pins

The library currently supports the following data pins:
- **Arduino/ESP32**: 2, 3, 4, 5, 11, 12, 13

---

## 🔧 Advanced Examples

### Color Cycling

```cpp
void loop() {
  static unsigned long lastChange = 0;
  static int colorIndex = 0;
  String colors[] = {"red", "green", "blue", "yellow", "purple", "cyan"};
  
  lumi.update();
  
  // Change color every 2 seconds
  if (millis() - lastChange > 2000) {
    lumi.setColor(colors[colorIndex]);
    colorIndex = (colorIndex + 1) % 6;
    lastChange = millis();
  }
}
```

### Palette Rotation

```cpp
void loop() {
  static unsigned long lastPalette = 0;
  static int paletteIndex = 0;
  String palettes[] = {"rainbow", "party", "ocean", "fire", "cyberpunk"};
  
  lumi.update();
  
  // Change palette every 10 seconds
  if (millis() - lastPalette > 10000) {
    lumi.setPalette(palettes[paletteIndex]);
    paletteIndex = (paletteIndex + 1) % 5;
    lastPalette = millis();
  }
}
```

### JSON Status Monitoring

```cpp
void loop() {
  static unsigned long lastStatus = 0;
  
  lumi.update();
  
  // Print status every 5 seconds
  if (millis() - lastStatus > 5000) {
    Serial.println("Current Status:");
    Serial.println(lumi.getStatus());
    lastStatus = millis();
  }
}
```

**Example JSON Output**:
```json
{
  "switch":"on",
  "bright":3,
  "fade":"on",
  "palette":"rainbow",
  "power":{"v":5,"ma":500},
  "blend_spd":4
}
```

---

## ⚡ Performance & Memory

### Performance Tips
1. **Optimize LED Count**: More LEDs require more RAM and processing
2. **Adjust Blend Speed**: Lower speeds reduce CPU usage
3. **Disable Unused Features**: Comment out unused palettes to save memory
4. **Power Management**: Always set appropriate power limits

---

## 🔧 Configuration Options

### Brightness Levels
```cpp
Level 1: 26/255  (10% - Night light)
Level 2: 64/255  (25% - Ambient)  
Level 3: 128/255 (50% - Normal)
Level 4: 192/255 (75% - Bright)
Level 5: 255/255 (100% - Maximum)
```

### Blend Speed Levels
```cpp
Level 1: 200ms, 25 max changes  (Slowest)
Level 2: 100ms, 50 max changes  (Slow)
Level 3: 50ms, 75 max changes   (Medium) 
Level 4: 25ms, 100 max changes  (Fast)
Level 5: 10ms, 150 max changes  (Fastest)
```

---

## 🛠️ Troubleshooting

### Common Issues

**LEDs not lighting up:**
- Check wiring connections (Data, Power, Ground)
- Verify data pin number matches code
- Ensure adequate power supply
- Check LED strip type (WS2812B supported)

**Colors appear wrong:**
- Try different color order in FastLED.addLeds (GRB, RGB, etc.)
- Check power supply voltage
- Verify LED strip specifications

**Flickering or unstable behavior:**
- Add capacitor across power supply (1000µF recommended)
- Check power supply current rating
- Reduce brightness if power limited
- Add resistor (300-500Ω) on data line

**Memory issues:**
- Reduce number of LEDs
- Disable unused palettes
- Lower blend speed settings

---

## 🎯 Use Cases

### Home Automation
- Ambient room lighting
- Status indicators
- Smart home integration
- Voice-controlled lighting

### Art & Entertainment  
- LED art installations
- Music visualization
- Interactive displays
- Stage lighting effects

### Holiday & Seasonal
- Christmas decorations
- Halloween effects
- Party lighting
- Holiday displays

### Educational Projects
- Arduino learning projects
- Color theory demonstrations
- Programming tutorials
- STEM education

---

## 🗂️ Library Structure

```
AvantLumi/
├── AvantLumi.h          # Header file with class definitions
├── AvantLumi.cpp        # Implementation file
├── examples/            # Example sketches
├── README.md            # This file
└── library.properties   # Arduino library metadata
```
---

## 📜 License

MIT License - See [LICENSE](LICENSE) for details.

---

**💡 Check out our other Arduino libraries at [AvantMaker GitHub](https://github.com/AvantMaker)!**

*Happy Making! 🛠️*