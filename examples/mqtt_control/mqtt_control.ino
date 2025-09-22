/*
 * AvantLumi MQTT Control Example
 * 
 * By: AvantMaker.com
 * Date: August, 2025
 * Version: 1.0.1
 * This example demonstrates how to control LED strips using the AvantLumi library
 * with MQTT commands. The LED status is automatically published at regular intervals.
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
 */

 #include "AvantLumi.h"
 #include <WiFi.h>
 #include <PubSubClient.h>
 #include <ArduinoJson.h>
 
 // ================================
 // CONFIGURATION
 // ================================
 // LED Configuration
 #define LED_DATA_PIN 5
 #define NUM_LEDS 30
 
 // WiFi credentials - Update these with your network details
 const char* ssid = "YOUR_WIFI_SSID";
 const char* password = "YOUR_WIFI_PASSWORD";
 
 // MQTT Broker settings
 const char* mqtt_server = "your.mqtt.broker.com";
 const int mqtt_port = 1883;
 const char* mqtt_user = "your_mqtt_user";
 const char* mqtt_password = "your_mqtt_password";
 
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
 
 // Timing variables
 unsigned long lastStatusReport = 0;
 
 // ================================
 // SETUP FUNCTION
 // ================================
 void setup() {
     Serial.begin(115200);
     Serial.println("AvantLumi MQTT Controller Starting...");
     
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
     
     Serial.println("Setup completed!");
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
     
     mqttClient.loop();
     
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
 // AVAILABLE COMMANDS REFERENCE
 // ================================
 /*
  * Command Examples:
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