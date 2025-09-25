/*
 * AvantLumi MQTT and Webpage Control Example
 * 
 * By: AvantMaker.com
 * Date: August, 2025
 * Version: 1.0.0
 * This example demonstrates how to control LED strips using both MQTT commands and a web interface.
 * The ESP32 serves as both an MQTT client and a web server, providing dual control methods.
 * 
 * This is a complete standalone solution that combines MQTT functionality with a web server,
 * allowing users to control LEDs through either MQTT messages or a web dashboard.
 * 
 * SETUP INSTRUCTIONS:
 * 1. Connect your LED strip to the ESP32 (default pin is 5)
 * 2. Update the WiFi credentials in the configuration section below
 * 3. Update the MQTT broker settings if needed
 * 4. Upload this sketch to your ESP32
 * 5. Open the Serial Monitor to see the connection details
 * 6. Connect to the IP address shown in the Serial Monitor using a web browser
 * 
 * MQTT Control Commands:
 * - switch:on/off    - Turn LED strip ON/OFF
 * - bright:1-5       - Set brightness level (1=lowest, 5=highest)
 * - fade:on/off      - Enable/disable fade-in effects
 * - rgb:255,128,64   - Set solid RGB color (R,G,B values)
 * - color:red        - Set solid color using color names
 * - palette:rainbow  - Set color palette (rainbow, party, ocean, etc.)
 * - blend_spd:1-5    - Set palette blending speed
 * - power:5,1000     - Set max power (volts,milliamps)
 * - config:save      - Save current settings to EEPROM
 * - config:load      - Load settings from EEPROM
 * - status           - Request immediate status report
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
 * - Real-time status updates including MQTT connection status
 * 
 * USAGE NOTES:
 * - The web interface is responsive and works on both desktop and mobile devices
 * - All settings are applied immediately when changed
 * - Configuration can be saved to EEPROM and restored later
 * - The web page is stored in PROGMEM to minimize RAM usage
 * - The ESP32 serves as both the web server and LED controller
 * - MQTT commands and web interface controls are synchronized
 * - Status updates are sent via MQTT and available through the web interface
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
 - PubSubClient.h
 * - ArduinoJson.h
 * - AvantLumi.h
 */

#include "AvantLumi.h"
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
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

// MQTT Broker settings
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";

// MQTT Topics
const char* led_control_topic = "avantmaker/avantlumi/control";
const char* led_status_topic = "avantmaker/avantlumi/status";

// Status reporting interval (seconds)
const uint16_t STATUS_REPORT_INTERVAL = 300;

// ================================
// GLOBAL OBJECTS
// ================================
AvantLumi ledController(LED_DATA_PIN, NUM_LEDS);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
WebServer server(80);

// Timing variables
unsigned long lastStatusReport = 0;

// ================================
// SETUP FUNCTION
// ================================
void setup() {
  Serial.begin(115200);
  Serial.println("AvantLumi MQTT and Webpage Controller Starting...");
  
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
  
  // Setup MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(onMqttMessage);
  
  // Setup web server routes
  setupWebServer();
  
  Serial.println("Setup completed!");
  Serial.print("Web server running at http://");
  Serial.println(WiFi.localIP());
  Serial.print("MQTT server: ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);
}

// ================================
// MAIN LOOP
// ================================
void loop() {
  // Ensure WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    setupWiFi();
  }
  
  // Ensure MQTT connection
  if (!mqttClient.connected()) {
    reconnectMqtt();
  }
  
  // Handle MQTT messages
  mqttClient.loop();
  
  // Handle web server clients
  server.handleClient();
  
  // Update LED controller
  ledController.update();
  
  // Handle periodic status reporting
  handleStatusReporting();
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
// MQTT CONNECTION
// ================================
void reconnectMqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "AvantLumi_" + String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println(" connected");
      
      // Subscribe to control topic
      mqttClient.subscribe(led_control_topic);
      Serial.print("Subscribed to: ");
      Serial.println(led_control_topic);
      
      // Send initial status
      sendStatus();
      
      // Send welcome message
      String welcomeMsg = "AvantLumi Controller Online! Device: " + clientId;
      mqttClient.publish(led_status_topic, welcomeMsg.c_str());
      
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

// ================================
// MQTT MESSAGE CALLBACK
// ================================
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("MQTT message received: ");
  Serial.println(message);
  
  // Process the command
  bool commandProcessed = processCommand(message);
  
  if (commandProcessed) {
    // Send status update after successful command
    sendStatus();
  } else {
    // Send error message
    String errorMsg = "Error: Unknown command '" + message + "'";
    mqttClient.publish(led_status_topic, errorMsg.c_str());
  }
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
  // Get current status as JSON with MQTT connection info
  String statusJson = getEnhancedStatus();
  server.send(200, "application/json", statusJson);
}

// ================================
// COMMAND PROCESSING
// ================================
bool processCommand(String command) {
  command.trim();
  command.toLowerCase();
  
  // Parse command and value
  int colonIndex = command.indexOf(':');
  if (colonIndex == -1 && command != "status") {
    return false;
  }
  
  String cmd, value;
  if (colonIndex != -1) {
    cmd = command.substring(0, colonIndex);
    value = command.substring(colonIndex + 1);
    cmd.trim();
    value.trim();
  } else {
    cmd = command;
  }
  
  // Process commands
  if (cmd == "switch") {
    return ledController.setSwitch(value);
  }
  else if (cmd == "bright") {
    uint8_t level = value.toInt();
    return ledController.setBright(level);
  }
  else if (cmd == "fade") {
    return ledController.setFade(value);
  }
  else if (cmd == "rgb") {
    // Parse RGB values: "255,128,64"
    int comma1 = value.indexOf(',');
    int comma2 = value.indexOf(',', comma1 + 1);
    
    if (comma1 != -1 && comma2 != -1) {
      uint8_t r = value.substring(0, comma1).toInt();
      uint8_t g = value.substring(comma1 + 1, comma2).toInt();
      uint8_t b = value.substring(comma2 + 1).toInt();
      return ledController.setRGB(r, g, b);
    }
    return false;
  }
  else if (cmd == "color") {
    return ledController.setColor(value);
  }
  else if (cmd == "palette") {
    return ledController.setPalette(value);
  }
  else if (cmd == "blend_spd") {
    uint8_t speed = value.toInt();
    return ledController.setBlendSpeed(speed);
  }
  else if (cmd == "power") {
    // Parse power values: "5,1000" (volts,milliamps)
    int commaIndex = value.indexOf(',');
    if (commaIndex != -1) {
      uint8_t volts = value.substring(0, commaIndex).toInt();
      uint32_t milliamps = value.substring(commaIndex + 1).toInt();
      return ledController.setMaxPower(volts, milliamps);
    }
    return false;
  }
  else if (cmd == "config" && value == "save") {
    bool success = ledController.saveConfig();
    String msg = success ? "Configuration saved to EEPROM" : "Failed to save configuration";
    mqttClient.publish(led_status_topic, msg.c_str());
    return success;
  }
  else if (cmd == "config" && value == "load") {
    bool success = ledController.loadConfig();
    String msg = success ? "Configuration loaded from EEPROM" : "Failed to load configuration";
    mqttClient.publish(led_status_topic, msg.c_str());
    return success;
  }
  else if (cmd == "status") {
    // Status will be sent by the calling function
    return true;
  }
  
  return false;
}

// ================================
// STATUS REPORTING
// ================================
void sendStatus() {
  if (mqttClient.connected()) {
    String statusJson = ledController.getStatus();
    
    if (mqttClient.publish(led_status_topic, statusJson.c_str(), true)) {
      Serial.println("Status sent: " + statusJson);
    } else {
      Serial.println("Failed to publish status!");
    }
  }
}

void handleStatusReporting() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastStatusReport >= (STATUS_REPORT_INTERVAL * 1000UL)) {
    lastStatusReport = currentTime;
    sendStatus();
    
    // Also send a heartbeat message
    String heartbeat = "Heartbeat: " + String(currentTime / 1000) + "s uptime, Free RAM: " + String(ESP.getFreeHeap()) + " bytes";
    mqttClient.publish(led_status_topic, heartbeat.c_str());
  }
}

// ================================
// ENHANCED STATUS (with MQTT info)
// ================================
String getEnhancedStatus() {
  // Get the basic status from the LED controller
  String basicStatus = ledController.getStatus();
  
  // Parse the JSON to add MQTT connection info
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, basicStatus);
  
  if (error) {
    // If parsing fails, return a simple status with MQTT info
    DynamicJsonDocument simpleDoc(256);
    simpleDoc["error"] = "Failed to parse LED status";
    simpleDoc["mqtt"]["connected"] = mqttClient.connected();
    simpleDoc["mqtt"]["server"] = mqtt_server;
    
    String output;
    serializeJson(simpleDoc, output);
    return output;
  }
  
  // Add MQTT connection info
  JsonObject mqtt = doc.createNestedObject("mqtt");
  mqtt["connected"] = mqttClient.connected();
  mqtt["server"] = mqtt_server;
  mqtt["port"] = mqtt_port;
  mqtt["control_topic"] = led_control_topic;
  mqtt["status_topic"] = led_status_topic;
  
  String output;
  serializeJson(doc, output);
  return output;
}

// ================================
// AVAILABLE COMMANDS REFERENCE
// ================================
/*
 * MQTT Command Examples:
 * 
 * Basic Control:
 * - switch:on          - Turn LED strip on
 * - switch:off         - Turn LED strip off
 * - bright:3           - Set brightness to level 3 (1-5)
 * - fade:on            - Enable fade effects
 * - fade:off           - Disable fade effects
 * 
 * Color Control:
 * - rgb:255,0,0        - Set solid red color
 * - rgb:128,255,64     - Set custom RGB color
 * - color:blue         - Set solid blue using color name
 * - color:hotpink      - Set solid hot pink
 * - color:forestgreen  - Set solid forest green
 * 
 * Palette Control:
 * - palette:rainbow    - Rainbow colors
 * - palette:party      - Party colors
 * - palette:ocean      - Ocean colors
 * - palette:heat       - Heat colors
 * - palette:christmas  - Christmas theme (red/green)
 * - palette:halloween  - Halloween theme (orange/purple)
 * - palette:cyberpunk  - Cyberpunk theme (pink/cyan)
 * - palette:random     - Random palette generation
 * 
 * Advanced Settings:
 * - blend_spd:3        - Set blending speed (1=slow, 5=fast)
 * - power:5,1000       - Set max power to 5V, 1000mA
 * 
 * Configuration:
 * - config:save        - Save current settings to EEPROM
 * - config:load        - Load settings from EEPROM
 * - status             - Get immediate status report
 * 
 * Web Control Examples (sent via HTTP GET requests):
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
 *   "blend_spd": 3,
 *   "mqtt": {
 *     "connected": true,
 *     "server": "test.mosquitto.org",
 *     "port": 1883,
 *     "control_topic": "avantmaker/avantlumi/control",
 *     "status_topic": "avantmaker/avantlumi/status"
 *   }
 * }
 * 
 * Or for solid colors:
 * {
 *   "switch": "on",
 *   "bright": 3,
 *   "fade": "on",
 *   "rgb": {"r": 255, "g": 0, "b": 0, "color": "red"},
 *   "power": {"v": 5, "ma": 1000},
 *   "blend_spd": 3,
 *   "mqtt": {
 *     "connected": true,
 *     "server": "test.mosquitto.org",
 *     "port": 1883,
 *     "control_topic": "avantmaker/avantlumi/control",
 *     "status_topic": "avantmaker/avantlumi/status"
 *   }
 * }
 */