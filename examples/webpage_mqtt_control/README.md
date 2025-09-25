# AvantLumi MQTT and Webpage Control Example

## Overview

This example demonstrates how to control LED strips using both MQTT commands and a web interface. The ESP32 serves as both an MQTT client and a web server, providing dual control methods for your LED strips.

This is a complete standalone solution that combines MQTT functionality with a web server, allowing users to control LEDs through either MQTT messages or a web dashboard. Both control methods are synchronized, so changes made through one interface are reflected in the other.

## Features

### MQTT Control
- **Power Control**: Turn LED strip ON/OFF
- **Brightness Control**: 5-level brightness adjustment
- **Fade Effects**: Enable/disable fade transitions
- **RGB Color Control**: Set custom RGB colors
- **Color Selection**: Choose from predefined color names
- **Color Palettes**: Select from various color palettes (rainbow, party, ocean, etc.)
- **Blend Speed**: Adjust color transition speed
- **Configuration Management**: Save and load settings to/from EEPROM
- **Status Reporting**: Automatic status updates at regular intervals

### Web Interface Control
- **Power Switch**: Toggle the LED strip on/off
- **Brightness Slider**: Adjust brightness from level 1 (dim) to 5 (bright)
- **Fade Toggle**: Enable/disable fade effects
- **RGB Inputs**: Enter custom RGB values (0-255 for each channel)
- **Color Dropdown**: Select from predefined colors
- **Palette Dropdown**: Choose from various color palettes
- **Blend Speed Slider**: Adjust color transition speed
- **Save/Load Config Buttons**: Save or load settings to/from EEPROM
- **MQTT Status Indicator**: Shows MQTT connection status
- **Real-time Status Updates**: Displays current LED and system status

## Hardware Requirements

- ESP32 development board
- LED strip (WS2812B or similar)
- Stable WiFi connection
- Adequate power supply for the LED strip

## Software Dependencies

- WiFi.h
- WebServer.h
- PubSubClient.h
- ArduinoJson.h
- AvantLumi.h

## Setup Instructions

1. **Hardware Setup**:
   - Connect your LED strip to the ESP32 (default pin is 5)
   - Ensure adequate power supply for the LED strip

2. **Software Configuration**:
   - Open the `mqtt_and_webpage_control.ino` file
   - Update the WiFi credentials in the configuration section:
     ```cpp
     const char* ssid = "YOUR_WIFI_SSID";        // UPDATE: Your WiFi network name
     const char* password = "YOUR_WIFI_PASSWORD"; // UPDATE: Your WiFi password
     ```
   - Update the MQTT broker settings if needed:
     ```cpp
     const char* mqtt_server = "test.mosquitto.org";
     const int mqtt_port = 1883;
     const char* mqtt_user = "";
     const char* mqtt_password = "";
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

## Usage

### Web Interface Usage

1. Open a web browser and navigate to the IP address shown in the Serial Monitor
2. Use the web interface to control your LED strip:
   - Toggle the power switch to turn the LED strip on/off
   - Adjust the brightness slider to change brightness levels
   - Enable/disable fade effects
   - Set custom RGB colors using the RGB input fields
   - Select predefined colors from the dropdown
   - Choose color palettes for dynamic effects
   - Adjust blend speed for palette transitions
   - Save/load configuration settings

### MQTT Usage

1. Connect an MQTT client to the same MQTT broker
2. Subscribe to the status topic to receive updates: `avantmaker/avantlumi/status`
3. Publish commands to the control topic: `avantmaker/avantlumi/control`

## API Reference

### MQTT Commands

#### Basic Control
- `switch:on` - Turn LED strip on
- `switch:off` - Turn LED strip off
- `bright:3` - Set brightness to level 3 (1-5)
- `fade:on` - Enable fade effects
- `fade:off` - Disable fade effects

#### Color Control
- `rgb:255,0,0` - Set solid red color
- `rgb:128,255,64` - Set custom RGB color
- `color:blue` - Set solid blue using color name
- `color:hotpink` - Set solid hot pink
- `color:forestgreen` - Set solid forest green

#### Palette Control
- `palette:rainbow` - Rainbow colors
- `palette:party` - Party colors
- `palette:ocean` - Ocean colors
- `palette:heat` - Heat colors
- `palette:christmas` - Christmas theme (red/green)
- `palette:halloween` - Halloween theme (orange/purple)
- `palette:cyberpunk` - Cyberpunk theme (pink/cyan)
- `palette:random` - Random palette generation

#### Advanced Settings
- `blend_spd:3` - Set blending speed (1=slow, 5=fast)
- `power:5,1000` - Set max power to 5V, 1000mA

#### Configuration
- `config:save` - Save current settings to EEPROM
- `config:load` - Load settings from EEPROM
- `status` - Get immediate status report

### Web API Endpoints

#### Root Page
- **URL**: `http://<ESP32_IP>/`
- **Method**: GET
- **Description**: Serves the main web interface

#### Control Endpoint
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

#### Status Endpoint
- **URL**: `http://<ESP32_IP>/status`
- **Method**: GET
- **Description**: Get current LED and MQTT status as JSON
- **Response Format**:
  ```json
  {
    "switch": "on",
    "bright": 3,
    "fade": "on",
    "palette": "rainbow",
    "power": {"v": 5, "ma": 1000},
    "blend_spd": 3,
    "mqtt": {
      "connected": true,
      "server": "test.mosquitto.org",
      "port": 1883,
      "control_topic": "avantmaker/avantlumi/control",
      "status_topic": "avantmaker/avantlumi/status"
    }
  }
  ```

## Troubleshooting

### WiFi Connection Issues
- Verify WiFi credentials are correct
- Ensure the ESP32 is within range of your WiFi router
- Check if your WiFi network is operational

### LED Strip Not Working
- Verify the LED strip is properly connected to the correct pin
- Ensure adequate power supply for the LED strip
- Check if the LED strip is compatible with the library

### Web Interface Not Loading
- Verify the ESP32 is connected to WiFi
- Check the Serial Monitor for the correct IP address
- Ensure you're accessing the correct IP address in your browser

### MQTT Connection Issues
- Verify MQTT broker settings are correct
- Check if the MQTT broker is accessible from your network
- Verify network connectivity between the ESP32 and MQTT broker
- Check MQTT broker authentication if required

### Configuration Not Saving
- Verify EEPROM is properly initialized
- Check if there's enough free space in EEPROM
- Try resetting the ESP32 after saving

## Integration with Home Automation

This example can be easily integrated with home automation systems:

### Home Assistant
1. Use the MQTT integration in Home Assistant
2. Configure the MQTT broker to match your settings
3. Create MQTT entities for control and status monitoring

### Node-RED
1. Use the MQTT nodes in Node-RED
2. Create flows to control the LED strip based on various triggers
3. Build custom dashboards for visual control

### OpenHAB
1. Use the MQTT binding in OpenHAB
2. Configure items for LED control and status
3. Create sitemaps for user interface

## Customization

The web interface can be customized by modifying the `webpage.h` file. You can:
- Change the appearance and layout
- Add new controls or features
- Modify the JavaScript behavior
- Adjust the CSS styling

The MQTT behavior can be customized by modifying the `mqtt_and_webpage_control.ino` file:
- Change MQTT topics
- Add custom commands
- Modify status reporting intervals
- Add additional sensors or data to status reports

## License

This example is part of the AvantLumi library and is subject to the same license terms.