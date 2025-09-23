# AvantLumi Web Dashboard & MQTT Control Examples

This Library's example directory contains two complementary examples that demonstrate how to create a web dashboard to control LED strips using MQTT:

1. **web_dashboard.ino** - Creates a web server interface for controlling LEDs
2. **mqtt_control.ino** - Controls LED strips based on MQTT commands

## Overview

These examples work together to provide a complete solution for controlling LED strips:
- The **web_dashboard.ino** runs on one ESP32 and provides a web interface
- The **mqtt_control.ino** runs on another ESP32 and controls the LED strip
- Communication between the two devices happens via MQTT messages

## Hardware Requirements

- Two ESP32 development boards
- LED strip (WS2812B or similar)
- Stable WiFi connection
- Access to an MQTT broker (public or private)

## Setup Instructions

### Step 1: Configure the MQTT Control Example

1. Open `mqtt_control.ino` in the Arduino IDE
2. Update the WiFi credentials:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";        // Your WiFi network name
   const char* password = "YOUR_WIFI_PASSWORD"; // Your WiFi password
   ```
3. Update the MQTT broker settings:
   ```cpp
   const char* mqtt_server = "test.mosquitto.org";  // Your MQTT broker address
   const int mqtt_port = 1883;                      // Your MQTT broker port
   const char* mqtt_user = "your_mqtt_user";        // Your MQTT username (if required)
   const char* mqtt_password = "your_mqtt_password"; // Your MQTT password (if required)
   ```
4. Connect your LED strip to the ESP32 (default pin is 5)
5. Upload the sketch to the ESP32 that will control the LED strip

### Step 2: Configure the Web Dashboard Example

1. Open `web_dashboard.ino` in the Arduino IDE
2. Update the WiFi credentials (must be the same network as the MQTT control ESP32):
   ```cpp
   const char* ssid = "your_SSID";          // Your WiFi network name
   const char* password = "your_PASSWORD";  // Your WiFi password
   ```
3. Update the MQTT broker settings (must match the settings in mqtt_control.ino):
   ```cpp
   char mqtt_server[40] = "test.mosquitto.org";  // Your MQTT broker address
   int mqtt_port = 1883;                          // Your MQTT broker port
   char mqtt_user[40] = "";                       // Your MQTT username (if required)
   char mqtt_password[40] = "";                   // Your MQTT password (if required)
   ```
4. Ensure the MQTT topic matches the control topic in mqtt_control.ino:
   ```cpp
   char led_topic[100] = "avantmaker/avantlumi/control";
   ```
5. Upload the sketch to the ESP32 that will serve the web dashboard

### Step 3: Using the Web Dashboard

1. Open the Serial Monitor for both ESP32s to see the connection status
2. Once both devices are connected to WiFi and MQTT, note the IP address of the web dashboard ESP32
3. Open a web browser and enter the IP address of the web dashboard ESP32
4. Use the web interface to control the LED strip connected to the other ESP32

## Web Dashboard Features

### Manual Control
- Turn LED strip ON/OFF
- Set brightness level (1-5)
- Enable/disable fade effects
- Set solid RGB colors
- Choose from predefined color palettes
- Adjust palette blending speed
- Set maximum power limits

### Scheduling
- Create up to 6 programmable on/off schedules
- Set specific times for the LED to turn on or off
- Enable/disable individual schedules
- Automatic time synchronization via NTP

### Configuration
- Configure MQTT broker settings through the web interface
- Set time zone and daylight saving time options
- Save and load configuration files
- Upload/download full configuration as JSON

## MQTT Commands

The web dashboard sends the following MQTT commands to control the LED strip:

| Command | Description | Example |
|---------|-------------|---------|
| switch:on/off | Turn LED strip ON/OFF | `switch:on` |
| bright:1-5 | Set brightness level (1=lowest, 5=highest) | `bright:3` |
| fade:on/off | Enable/disable fade-in effects | `fade:on` |
| rgb:255,128,64 | Set solid RGB color | `rgb:255,0,0` |
| color:red | Set solid color using color name | `color:blue` |
| palette:rainbow | Set color palette | `palette:party` |
| blend_spd:1-5 | Set palette blending speed | `blend_spd:3` |
| power:5,1000 | Set max power (volts,milliamps) | `power:5,1000` |
| config:save | Save current settings to EEPROM | `config:save` |
| config:load | Load settings from EEPROM | `config:load` |
| status | Request immediate status report | `status` |

## Available Color Palettes

- rainbow - Rainbow colors
- party - Party colors
- ocean - Ocean colors
- heat - Heat colors
- christmas - Christmas theme (red/green)
- halloween - Halloween theme (orange/purple)
- cyberpunk - Cyberpunk theme (pink/cyan)
- random - Random palette generation

## Available Color Names

Common color names include: red, green, blue, yellow, orange, purple, pink, white, black, cyan, magenta, lime, maroon, navy, olive, teal, silver, gold, and many more.

## Troubleshooting

### Connection Issues
- Ensure both ESP32s are connected to the same WiFi network
- Verify MQTT broker settings are identical in both sketches
- Check that the MQTT broker is accessible from your network
- Make sure the MQTT topics match between the two sketches

### LED Control Issues
- Verify the LED strip is properly connected to the correct pin
- Check power supply - LED strips can require significant current
- Ensure the LED strip type is compatible with the AvantLumi library
- Monitor the Serial Monitor for error messages

### Web Dashboard Issues
- Ensure the ESP32 has a stable WiFi connection
- Try accessing the dashboard using the IP address shown in the Serial Monitor
- Clear browser cache if the interface doesn't load properly
- Check that LittleFS is properly formatted on the ESP32

## Advanced Usage

### Custom MQTT Brokers
While the examples use "test.mosquitto.org" as a default public broker, for production use consider:
- Setting up your own MQTT broker (e.g., Mosquitto, EMQX, HiveMQ)
- Using secure connections (MQTT over TLS)
- Implementing authentication and authorization

### Expanding the Web Dashboard
The web dashboard can be extended with:
- Additional LED control options
- Support for multiple LED strips
- User authentication
- Integration with home automation systems
- Mobile app compatibility

## Support

For more information about the AvantLumi library and additional examples, visit:
- AvantMaker.com
- The AvantLumi GitHub repository