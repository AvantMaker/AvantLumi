# AvantLumi Webpage Control Example

## Overview

This example demonstrates how to create a standalone web server to control LED strips using the AvantLumi library directly. The web interface provides an intuitive dashboard for controlling LEDs connected to the same ESP32 that is serving the web page.

This is a complete standalone solution that combines web server functionality with direct LED control, eliminating the need for a separate MQTT broker or second device.

## Features

- **Power Control**: Turn LED strip ON/OFF
- **Brightness Control**: 5-level brightness adjustment
- **Fade Effects**: Enable/disable fade transitions
- **RGB Color Control**: Set custom RGB colors
- **Color Selection**: Choose from predefined color names
- **Color Palettes**: Select from various color palettes (rainbow, party, ocean, etc.)
- **Blend Speed**: Adjust color transition speed
- **Configuration Management**: Save and load settings to/from EEPROM
- **Responsive Web Interface**: Works on both desktop and mobile devices

## Hardware Requirements

- ESP32 development board
- LED strip (WS2812B or similar)
- Stable WiFi connection
- Adequate power supply for the LED strip

## Software Dependencies

- WiFi.h
- WebServer.h
- ArduinoJson.h
- AvantLumi.h

## Setup Instructions

1. **Hardware Setup**:
   - Connect your LED strip to the ESP32 (default pin is 5)
   - Ensure adequate power supply for the LED strip

2. **Software Configuration**:
   - Open the `webpage_control.ino` file
   - Update the WiFi credentials in the configuration section:
     ```cpp
     const char* ssid = "YOUR_WIFI_SSID";        // UPDATE: Your WiFi network name
     const char* password = "YOUR_WIFI_PASSWORD"; // UPDATE: Your WiFi password
     ```
   - Adjust LED configuration if needed:
     ```cpp
     #define LED_DATA_PIN 5    // LED data pin
     #define NUM_LEDS 30       // Number of LEDs in your strip
     ```

3. **Upload and Run**:
   - Upload this sketch to your ESP32
   - Open the Serial Monitor to see the connection details
   - Connect to the IP address shown in the Serial Monitor using a web browser

## Web Interface Usage

The web interface provides the following controls:

- **Power Switch**: Toggle the LED strip on/off
- **Brightness Slider**: Adjust brightness from level 1 (dim) to 5 (bright)
- **Fade Toggle**: Enable/disable fade effects
- **RGB Inputs**: Enter custom RGB values (0-255 for each channel)
- **Color Dropdown**: Select from predefined colors
- **Palette Dropdown**: Choose from various color palettes
- **Blend Speed Slider**: Adjust color transition speed
- **Save Config Button**: Save current settings to EEPROM
- **Load Config Button**: Load previously saved settings

## API Reference

The web server provides the following endpoints:

### Root Page
- **URL**: `http://<ESP32_IP>/`
- **Method**: GET
- **Description**: Serves the main web interface

### Control Endpoint
- **URL**: `http://<ESP32_IP>/control`
- **Method**: GET
- **Description**: Apply control commands to the LED strip
- **Parameters**:
  - `switch`: on/off - Control power state
  - `bright`: 1-5 - Set brightness level
  - `fade`: on/off - Enable/disable fade effects
  - `rgb`: R,G,B - Set RGB color (e.g., 255,0,0 for red)
  - `color`: color_name - Set color by name (e.g., blue, green, red)
  - `palette`: palette_name - Set color palette (e.g., rainbow, party, ocean)
  - `blend_spd`: 1-5 - Set blend speed
  - `config`: save/load - Save or load configuration

### Status Endpoint
- **URL**: `http://<ESP32_IP>/status`
- **Method**: GET
- **Description**: Get current LED status as JSON
- **Response Format**:
  ```json
  {
    "switch": "on",
    "bright": 3,
    "fade": "on",
    "palette": "rainbow",
    "power": {"v": 5, "ma": 1000},
    "blend_spd": 3
  }
  ```

## Troubleshooting

1. **WiFi Connection Issues**:
   - Verify WiFi credentials are correct
   - Ensure the ESP32 is within range of your WiFi router
   - Check if your WiFi network is operational

2. **LED Strip Not Working**:
   - Verify the LED strip is properly connected to the correct pin
   - Ensure adequate power supply for the LED strip
   - Check if the LED strip is compatible with the library

3. **Web Interface Not Loading**:
   - Verify the ESP32 is connected to WiFi
   - Check the Serial Monitor for the correct IP address
   - Ensure you're accessing the correct IP address in your browser

4. **Configuration Not Saving**:
   - Verify EEPROM is properly initialized
   - Check if there's enough free space in EEPROM
   - Try resetting the ESP32 after saving

## Customization

The web interface can be customized by modifying the `webpage.h` file. You can:
- Change the appearance and layout
- Add new controls or features
- Modify the JavaScript behavior
- Adjust the CSS styling

## License

This example is part of the AvantLumi library and is subject to the same license terms.