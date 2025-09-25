/*
 * AvantLumi Webpage Control Example
 * 
 * By: AvantMaker.com
 * Date: August, 2025
 * Version: 1.0.0
 * This example demonstrates how to create a standalone web server to control LED strips
 * using the AvantLumi library directly. The web interface provides an intuitive dashboard
 * for controlling LEDs connected to the same ESP32 that is serving the web page.
 * 
 * This is a complete standalone solution that combines web server functionality with
 * direct LED control, eliminating the need for a separate MQTT broker or second device.
 * 
 * SETUP INSTRUCTIONS:
 * 1. Connect your LED strip to the ESP32 (default pin is 5)
 * 2. Update the WiFi credentials in the configuration section below
 * 3. Upload this sketch to your ESP32
 * 4. Open the Serial Monitor to see the connection details
 * 5. Connect to the IP address shown in the Serial Monitor using a web browser
 * 
 * Web Dashboard Features:
 * - Power switch to turn LED strip ON/OFF
 * - Brightness slider (1-5 levels)
 * - Fade effect toggle
 * - RGB color control with individual R,G,B inputs
 * - Color selection from predefined color names
 * - Color palette selection (rainbow, party, ocean, etc.)
 * - Blend speed slider (1-5 levels)
 * - Save/Load configuration buttons
 * - Real-time status updates
 * 
 * USAGE NOTES:
 * - The web interface is responsive and works on both desktop and mobile devices
 * - All settings are applied immediately when changed
 * - Configuration can be saved to EEPROM and restored later
 * - The web page is stored in PROGMEM to minimize RAM usage
 * - The ESP32 serves as both the web server and LED controller
 * 
 * HARDWARE REQUIREMENTS:
 * - ESP32 development board
 * - LED strip (WS2812B or similar)
 * - Stable WiFi connection
 * - Adequate power supply for the LED strip
 * 
 * DEPENDENCIES:
 * - WiFi.h
 * - WebServer.h
 * - ArduinoJson.h
 * - AvantLumi.h
 */

#include "AvantLumi.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "webpage.h"

// ================================
// CONFIGURATION
// ================================
// LED Configuration
#define LED_DATA_PIN 5
#define NUM_LEDS 30

// WiFi credentials - Update these with your network details
const char* ssid = "YOUR_WIFI_SSID";        // UPDATE: Your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD"; // UPDATE: Your WiFi password

// ================================
// GLOBAL OBJECTS
// ================================
AvantLumi ledController(LED_DATA_PIN, NUM_LEDS);
WebServer server(80);

// ================================
// SETUP FUNCTION
// ================================
void setup() {
  Serial.begin(115200);
  Serial.println("AvantLumi Webpage Controller Starting...");
  
  // Initialize LED controller
  if (!ledController.begin()) {
    Serial.println("Warning: LED pin not officially supported, using default pin 2");
  }
  
  // Try to load saved configuration
  if (ledController.checkConfig()) {
    if (ledController.loadConfig()) {
      Serial.println("Configuration loaded from EEPROM");
    } else {
      Serial.println("Failed to load configuration");
    }
  } else {
    Serial.println("No saved configuration found, using defaults");
  }
  
  // Connect to WiFi
  setupWiFi();
  
  // Setup web server routes
  setupWebServer();
  
  Serial.println("Setup completed!");
  Serial.print("Web server running at http://");
  Serial.println(WiFi.localIP());
}

// ================================
// MAIN LOOP
// ================================
void loop() {
  // Handle web server clients
  server.handleClient();
  
  // Update LED controller
  ledController.update();
}

// ================================
// WIFI SETUP
// ================================
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// ================================
// WEB SERVER SETUP
// ================================
void setupWebServer() {
  // Serve the main webpage
  server.on("/", HTTP_GET, handleRoot);
  
  // Handle control requests
  server.on("/control", HTTP_GET, handleControl);
  
  // Handle status requests
  server.on("/status", HTTP_GET, handleStatus);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

// ================================
// WEB PAGE HANDLER
// ================================
void handleRoot() {
  server.send_P(200, "text/html", webpage_html);
}

// ================================
// CONTROL HANDLER
// ================================
void handleControl() {
  String message = "Command processed";
  
  // Parse parameters
  if (server.hasArg("switch")) {
    String switchState = server.arg("switch");
    if (ledController.setSwitch(switchState)) {
      message = "Switch set to " + switchState;
    } else {
      message = "Failed to set switch";
    }
  }
  
  if (server.hasArg("bright")) {
    uint8_t brightness = server.arg("bright").toInt();
    if (ledController.setBright(brightness)) {
      message = "Brightness set to " + String(brightness);
    } else {
      message = "Failed to set brightness";
    }
  }
  
  if (server.hasArg("fade")) {
    String fadeState = server.arg("fade");
    if (ledController.setFade(fadeState)) {
      message = "Fade set to " + fadeState;
    } else {
      message = "Failed to set fade";
    }
  }
  
  if (server.hasArg("rgb")) {
    String rgbValue = server.arg("rgb");
    // Parse RGB values: "255,128,64"
    int comma1 = rgbValue.indexOf(',');
    int comma2 = rgbValue.indexOf(',', comma1 + 1);
    
    if (comma1 != -1 && comma2 != -1) {
      uint8_t r = rgbValue.substring(0, comma1).toInt();
      uint8_t g = rgbValue.substring(comma1 + 1, comma2).toInt();
      uint8_t b = rgbValue.substring(comma2 + 1).toInt();
      if (ledController.setRGB(r, g, b)) {
        message = "RGB color set to " + String(r) + "," + String(g) + "," + String(b);
      } else {
        message = "Failed to set RGB color";
      }
    } else {
      message = "Invalid RGB format";
    }
  }
  
  if (server.hasArg("color")) {
    String colorName = server.arg("color");
    if (ledController.setColor(colorName)) {
      message = "Color set to " + colorName;
    } else {
      message = "Failed to set color";
    }
  }
  
  if (server.hasArg("palette")) {
    String paletteName = server.arg("palette");
    if (ledController.setPalette(paletteName)) {
      message = "Palette set to " + paletteName;
    } else {
      message = "Failed to set palette";
    }
  }
  
  if (server.hasArg("blend_spd")) {
    uint8_t speed = server.arg("blend_spd").toInt();
    if (ledController.setBlendSpeed(speed)) {
      message = "Blend speed set to " + String(speed);
    } else {
      message = "Failed to set blend speed";
    }
  }
  
  if (server.hasArg("config")) {
    String configCmd = server.arg("config");
    if (configCmd == "save") {
      if (ledController.saveConfig()) {
        message = "Configuration saved to EEPROM";
      } else {
        message = "Failed to save configuration";
      }
    } else if (configCmd == "load") {
      if (ledController.loadConfig()) {
        message = "Configuration loaded from EEPROM";
      } else {
        message = "Failed to load configuration";
      }
    }
  }
  
  server.send(200, "text/plain", message);
}

// ================================
// STATUS HANDLER
// ================================
void handleStatus() {
  // Get current status as JSON
  String statusJson = ledController.getStatus();
  server.send(200, "application/json", statusJson);
}

// ================================
// AVAILABLE COMMANDS REFERENCE
// ================================
/*
 * Command Examples (sent via HTTP GET requests):
 * 
 * Basic Control:
 * - /control?switch=on     - Turn LED strip on
 * - /control?switch=off    - Turn LED strip off
 * - /control?bright=3      - Set brightness to level 3 (1-5)
 * - /control?fade=on        - Enable fade effects
 * - /control?fade=off       - Disable fade effects
 * 
 * Color Control:
 * - /control?rgb=255,0,0   - Set solid red color
 * - /control?rgb=128,255,64 - Set custom RGB color
 * - /control?color=blue    - Set solid blue using color name
 * - /control?color=hotpink  - Set solid hot pink
 * - /control?color=forestgreen - Set solid forest green
 * 
 * Palette Control:
 * - /control?palette=rainbow - Rainbow colors
 * - /control?palette=party   - Party colors
 * - /control?palette=ocean   - Ocean colors
 * - /control?palette=heat    - Heat colors
 * - /control?palette=christmas - Christmas theme (red/green)
 * - /control?palette=halloween - Halloween theme (orange/purple)
 * - /control?palette=cyberpunk - Cyberpunk theme (pink/cyan)
 * - /control?palette=random  - Random palette generation
 * 
 * Advanced Settings:
 * - /control?blend_spd=3    - Set blending speed (1=slow, 5=fast)
 * 
 * Configuration:
 * - /control?config=save    - Save current settings to EEPROM
 * - /control?config=load    - Load settings from EEPROM
 * 
 * Status JSON Format:
 * {
 *   "switch": "on",
 *   "bright": 3,
 *   "fade": "on",
 *   "palette": "rainbow",
 *   "power": {"v": 5, "ma": 1000},
 *   "blend_spd": 3
 * }
 * 
 * Or for solid colors:
 * {
 *   "switch": "on",
 *   "bright": 3,
 *   "fade": "on",
 *   "rgb": {"r": 255, "g": 0, "b": 0, "color": "red"},
 *   "power": {"v": 5, "ma": 1000},
 *   "blend_spd": 3
 * }
 */