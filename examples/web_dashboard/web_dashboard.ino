/*
 * AvantLumi Web Dashboard Example
 * 
 * By: AvantMaker.com
 * Date: August, 2025
 * Version: 0.0.11
 * This example demonstrates how to create a web dashboard to control LED strips
 * using the AvantLumi library with MQTT commands. The web dashboard provides
 * an intuitive interface for controlling LEDs connected to another ESP32 running
 * the mqtt_control.ino example.
 * 
 * IMPORTANT: This example is NOT designed to be used alone. It must be used in
 * conjunction with the mqtt_control.ino example. The web dashboard creates a web
 * server that sends MQTT commands to control the LEDs connected to the ESP32
 * running the mqtt_control.ino sketch.
 * 
 * SETUP INSTRUCTIONS:
 * 1. Upload this sketch to one ESP32 (will act as web dashboard and MQTT publisher)
 * 2. Upload mqtt_control.ino to another ESP32 (will control the LED strip)
 * 3. Configure both sketches with the same WiFi and MQTT broker settings
 * 4. Ensure the MQTT topic in this sketch (led_topic) matches the control topic
 *    in mqtt_control.ino (led_control_topic)
 * 5. Access the web dashboard by entering the IP address of this ESP32 in a browser
 * 
 * Web Dashboard Features:
 * - Manual LED control (ON/OFF)
 * - Up to 6 programmable on/off schedules for the LED
 * - MQTT broker configuration interface
 * - Time settings with NTP synchronization and timezone support
 * - Configuration save/load functionality
 * - Responsive web interface that works on both desktop and mobile
 * 
 * USAGE NOTES:
 * - The default MQTT topic is "avantmaker/avantlumi/control" which matches the
 *   default topic in mqtt_control.ino
 * - You must configure WiFi credentials and MQTT broker settings in both sketches
 * - The web dashboard sends the same MQTT commands as described in mqtt_control.ino:
 *   - switch:on/off    - Turn LED strip ON/OFF
 *   - bright:1-5       - Set brightness level (1=lowest, 5=highest)
 *   - fade:on/off      - Enable/disable fade-in effects
 *   - rgb:255,128,64   - Set solid RGB color (R,G,B values)
 *   - color:red        - Set solid color using color names
 *   - palette:rainbow  - Set color palette (rainbow, party, ocean, etc.)
 *   - blend_spd:1-5    - Set palette blending speed
 *   - power:5,1000     - Set max power (volts,milliamps)
 *   - config:save      - Save current settings to EEPROM
 *   - config:load      - Load settings from EEPROM
 *   - status           - Request immediate status report
 * - All configurations are saved to LittleFS for persistence
 * - The dashboard automatically synchronizes time via NTP for scheduling
 * 
 * HARDWARE REQUIREMENTS:
 * - ESP32 development board
 * - Stable WiFi connection
 * - Access to an MQTT broker (public or private)
 * 
 * DEPENDENCIES:
 * - WiFi.h
 * - WebServer.h
 * - PubSubClient.h
 * - ArduinoJson.h
 * - LittleFS.h
 * - time.h
 */

#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "time.h"

// --- Default Configuration ---
// IMPORTANT: Update these settings with your network and MQTT broker information
// These settings must match the configuration in mqtt_control.ino for proper operation
#define NTP_UPDATE_INTERVAL_MINUTES 30 // Default NTP update interval (user adjustable)
const char* ssid = "your_SSID";          // UPDATE: Your WiFi network name
const char* password = "your_PASSWORD";  // UPDATE: Your WiFi password

// Configuration file path
const char* CONFIG_FILE = "/full_config.json";

// MQTT Broker settings (can be changed via web UI)
// IMPORTANT: These must match the MQTT broker settings in mqtt_control.ino
char mqtt_server[40] = "test.mosquitto.org";  // UPDATE: Your MQTT broker address
int mqtt_port = 1883;                          // UPDATE: Your MQTT broker port
char mqtt_user[40] = "";                       // UPDATE: Your MQTT username (if required)
char mqtt_password[40] = "";                   // UPDATE: Your MQTT password (if required)

// MQTT Topics
// IMPORTANT: This must match the led_control_topic in mqtt_control.ino
char led_topic[100] = "avantmaker/avantlumi/control";

// NTP settings
const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 0;
int daylightOffset_sec = 3600;

// NTP update interval tracking
unsigned long lastNtpSyncMillis = 0;
const unsigned long ntpUpdateIntervalMs = NTP_UPDATE_INTERVAL_MINUTES * 60UL * 1000UL;

// --- Global Variables ---
WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
String clientId; // For MQTT client ID

struct Schedule {
  int hour;
  int minute;
  bool state; // true for ON, false for OFF
  bool enabled;
};

Schedule schedules[6];
bool use_dst = false;

// Configuration save flags for non-blocking operation
bool configNeedsSave = false;
unsigned long lastConfigSave = 0;
const unsigned long CONFIG_SAVE_DELAY = 2000; // Wait 2 seconds before saving

// --- Optimized LittleFS Configuration Management Functions ---
bool initLittleFS() {
  Serial.println("Initializing LittleFS...");
  if (!LittleFS.begin(false)) { // false = don't format if mount fails
    Serial.println("LittleFS mount failed, trying to format...");
    if (!LittleFS.begin(true)) { // true = format if needed
      Serial.println("LittleFS format failed");
      return false;
    }
  }
  
  // Check available space
  size_t totalBytes = LittleFS.totalBytes();
  size_t usedBytes = LittleFS.usedBytes();
  Serial.printf("LittleFS: %u total bytes, %u used bytes\n", totalBytes, usedBytes);
  
  return true;
}

void saveFullConfigNow() {
  Serial.println("Saving configuration to LittleFS...");
  
  // Create a smaller, more efficient JSON document
  DynamicJsonDocument doc(1536); // Reduced size for better memory management

  // Schedules - more compact format
  JsonArray schedulesArray = doc.createNestedArray("schedules");
  for (int i = 0; i < 6; i++) {
    JsonObject schedule = schedulesArray.createNestedObject();
    schedule["h"] = schedules[i].hour;
    schedule["m"] = schedules[i].minute;
    schedule["s"] = schedules[i].state;
    schedule["e"] = schedules[i].enabled;
  }

  // Time settings
  JsonObject timeSettings = doc.createNestedObject("time");
  timeSettings["gmt"] = gmtOffset_sec / 3600;
  timeSettings["dst"] = use_dst;

  // MQTT settings
  JsonObject mqttSettings = doc.createNestedObject("mqtt");
  mqttSettings["srv"] = mqtt_server;
  mqttSettings["prt"] = mqtt_port;
  mqttSettings["usr"] = mqtt_user;
  mqttSettings["pwd"] = mqtt_password;

  // LED settings
  JsonObject ledSettings = doc.createNestedObject("led");
  ledSettings["tpc"] = led_topic;

  File configFile = LittleFS.open(CONFIG_FILE, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return;
  }

  size_t bytesWritten = serializeJson(doc, configFile);
  configFile.close();
  
  Serial.printf("Configuration saved successfully (%u bytes)\n", bytesWritten);
}

void loadFullConfig() {
  Serial.println("Loading configuration from LittleFS...");
  
  File configFile = LittleFS.open(CONFIG_FILE, "r");
  if (!configFile) {
    Serial.println("Config file not found, creating default configuration");
    saveFullConfigNow(); // Create default config file
    return;
  }

  size_t fileSize = configFile.size();
  if (fileSize > 2048) { // Sanity check
    Serial.println("Config file too large, using defaults");
    configFile.close();
    return;
  }

  DynamicJsonDocument doc(1536);
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();

  if (error) {
    Serial.printf("Failed to parse config file: %s\n", error.c_str());
    saveFullConfigNow(); // Create new default config file
    return;
  }

  // Load schedules with compact format
  JsonArray schedulesArray = doc["schedules"];
  if (schedulesArray) {
    int i = 0;
    for (JsonVariant scheduleVar : schedulesArray) {
      if (i >= 6) break;
      JsonObject schedule = scheduleVar.as<JsonObject>();
      schedules[i].hour = schedule["h"] | 12;
      schedules[i].minute = schedule["m"] | 0;
      schedules[i].state = schedule["s"] | false;
      schedules[i].enabled = schedule["e"] | false;
      i++;
    }
  }

  // Load time settings
  JsonObject timeSettings = doc["time"];
  if (timeSettings) {
    gmtOffset_sec = (timeSettings["gmt"] | 0) * 3600;
    use_dst = timeSettings["dst"] | false;
    daylightOffset_sec = use_dst ? 3600 : 0;
  }

  // Load MQTT settings
  JsonObject mqttSettings = doc["mqtt"];
  if (mqttSettings) {
    strlcpy(mqtt_server, mqttSettings["srv"] | "broker.emqx.io", sizeof(mqtt_server));
    mqtt_port = mqttSettings["prt"] | 1883;
    strlcpy(mqtt_user, mqttSettings["usr"] | "", sizeof(mqtt_user));
    strlcpy(mqtt_password, mqttSettings["pwd"] | "", sizeof(mqtt_password));
  }

  // Load LED settings
  JsonObject ledSettings = doc["led"];
  if (ledSettings) {
    strlcpy(led_topic, ledSettings["tpc"] | "AvantMaker/esp32/led", sizeof(led_topic));
  }

  Serial.println("Configuration loaded successfully");
}

// Non-blocking configuration save
void scheduleConfigSave() {
  configNeedsSave = true;
  lastConfigSave = millis();
}

void processConfigSave() {
  if (configNeedsSave && (millis() - lastConfigSave > CONFIG_SAVE_DELAY)) {
    saveFullConfigNow();
    configNeedsSave = false;
  }
}

// --- Helper Functions ---
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed");
  }
}

void reconnectMqtt() {
  // Non-blocking MQTT connection attempt
  static unsigned long lastMqttAttempt = 0;
  
  if (!client.connected() && (millis() - lastMqttAttempt > 5000)) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
    lastMqttAttempt = millis();
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 1000)) { // 1 second timeout
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void checkSchedules() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 100)) { // Short timeout to avoid blocking
    return;
  }

  static int lastMinute = -1;
  if (timeinfo.tm_min == lastMinute) {
    return; // Already checked this minute
  }
  lastMinute = timeinfo.tm_min;

  for (int i = 0; i < 6; i++) {
    if (schedules[i].enabled && 
        schedules[i].hour == timeinfo.tm_hour && 
        schedules[i].minute == timeinfo.tm_min) {
      
      Serial.printf("Schedule %d triggered: LED %s\n", i, schedules[i].state ? "ON" : "OFF");
      if (client.connected()) {
        client.publish(led_topic, schedules[i].state ? "switch:on" : "switch:off");
      }
    }
  }
}

// --- Optimized Webpage Handlers ---
String getPageHeader(const String& title) {
  String header;
  header.reserve(1200); // Pre-allocate memory
  
  header = "<!DOCTYPE html><html><head><title>" + title + "</title>";
  header += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  header += "<style>";
  header += "body{font-family:Arial,sans-serif;background:#f4f4f4;color:#333;margin:0;padding:0;}";
  header += ".container{max-width:800px;margin:0 auto;background:#fff;padding:20px;box-shadow:0 2px 5px rgba(0,0,0,0.1);}";
  header += "h1,h2{color:#0056b3;margin-top:0;}";
  header += "nav{background:#0056b3;padding:10px;text-align:center;margin:-20px -20px 20px -20px;}";
  header += "nav a{color:white;padding:8px 12px;text-decoration:none;display:inline-block;font-size:14px;}";
  header += "nav a:hover{background:#004494;}";
  header += "input,select{width:100%;padding:8px;margin:4px 0;border:1px solid #ccc;border-radius:4px;box-sizing:border-box;}";
  header += "button{background:#4CAF50;color:white;padding:10px 15px;margin:5px 0;border:none;cursor:pointer;width:100%;border-radius:4px;font-size:14px;}";
  header += "button:hover{background:#45a049;}";
  header += ".btn-danger{background:#f44336;}.btn-danger:hover{background:#da190b;}";
  header += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:10px;margin:10px 0;}";
  header += ".card{background:#f9f9f9;padding:15px;border:1px solid #eee;border-radius:4px;}";
  header += "@media(max-width:600px){.container{padding:10px;}}";
  header += "</style></head><body>";
  header += "<div class='container'>";
  header += "<nav><a href='/'>Home</a><a href='/schedules'>Schedules</a><a href='/timesettings'>Time</a><a href='/mqttsettings'>MQTT</a><a href='/ledsettings'>LED</a></nav>";
  header += "<h1>" + title + "</h1>";
  
  return header;
}

String getPageFooter() {
  return "</div></body></html>";
}

void handleRoot() {
  String page;
  page.reserve(2000); // Pre-allocate memory
  
  page = getPageHeader("ESP32 Control Panel");
  
  // Get current time efficiently
  struct tm timeinfo;
  bool timeValid = getLocalTime(&timeinfo, 500);
  
  page += "<h2>Status</h2><div class='grid'>";
  page += "<div class='card'><strong>WiFi:</strong> " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "</div>";
  page += "<div class='card'><strong>IP:</strong> " + WiFi.localIP().toString() + "</div>";
  page += "<div class='card'><strong>MQTT:</strong> " + String(client.connected() ? "Connected" : "Disconnected") + "</div>";
  
  if (timeValid) {
    char timeString[25];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
    page += "<div class='card'><strong>Time:</strong> " + String(timeString) + "</div>";
  } else {
    page += "<div class='card'><strong>Time:</strong> Not Available</div>";
  }
  page += "</div>";

  page += "<h2>Manual Control</h2>";
  page += "<a href='/led_on'><button>Turn LED ON</button></a>";
  page += "<a href='/led_off'><button class='btn-danger'>Turn LED OFF</button></a>";
  
  page += "<h2>Configuration</h2>";
  page += "<a href='/download_full_config'><button>Download Full Config</button></a>";
  page += "<form method='post' action='/upload_full_config' enctype='multipart/form-data'>";
  page += "<input type='file' name='upload' accept='.json'>";
  page += "<button type='submit'>Upload Full Config</button></form>";

  page += getPageFooter();
  server.send(200, "text/html", page);
}

void handleLedOn() {
  if (client.connected()) {
    client.publish(led_topic, "switch:on");
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleLedOff() {
  if (client.connected()) {
    client.publish(led_topic, "switch:off");
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleDownloadFullConfig() {
  File configFile = LittleFS.open(CONFIG_FILE, "r");
  if (!configFile) {
    server.send(500, "text/plain", "Configuration file not found");
    return;
  }

  server.sendHeader("Content-Disposition", "attachment; filename=full_config.json");
  server.streamFile(configFile, "application/json"); // Stream file directly
  configFile.close();
}

void handleUploadFullConfig() {
  HTTPUpload& upload = server.upload();
  static File uploadFile;
  
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Upload started: %s\n", upload.filename.c_str());
    uploadFile = LittleFS.open("/temp_config.json", "w");
    if (!uploadFile) {
      Serial.println("Failed to open temp file for upload");
      return;
    }
  } 
  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
  } 
  else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
      
      // Process the uploaded file
      uploadFile = LittleFS.open("/temp_config.json", "r");
      if (uploadFile) {
        DynamicJsonDocument doc(1536);
        DeserializationError error = deserializeJson(doc, uploadFile);
        uploadFile.close();
        
        if (!error) {
          // Apply configuration immediately
          applyUploadedConfig(doc);
          
          // Replace the main config file
          LittleFS.remove(CONFIG_FILE);
          LittleFS.rename("/temp_config.json", CONFIG_FILE);
          
          Serial.println("Full configuration uploaded and applied");
        } else {
          Serial.printf("JSON parse error: %s\n", error.c_str());
          LittleFS.remove("/temp_config.json");
        }
      }
    }
  }
}

void applyUploadedConfig(DynamicJsonDocument& doc) {
  // Apply schedules
  JsonArray schedulesArray = doc["schedules"];
  if (schedulesArray) {
    int i = 0;
    for (JsonVariant scheduleVar : schedulesArray) {
      if (i >= 6) break;
      JsonObject schedule = scheduleVar.as<JsonObject>();
      schedules[i].hour = schedule["h"] | schedule["hour"] | 12;
      schedules[i].minute = schedule["m"] | schedule["minute"] | 0;
      schedules[i].state = schedule["s"] | schedule["state"] | false;
      schedules[i].enabled = schedule["e"] | schedule["enabled"] | false;
      i++;
    }
  }

  // Apply time settings
  JsonObject timeSettings = doc["time"];
  if (timeSettings) {
    gmtOffset_sec = (timeSettings["gmt"] | timeSettings["gmtOffset"] | 0) * 3600;
    use_dst = timeSettings["dst"] | timeSettings["use_dst"] | false;
    daylightOffset_sec = use_dst ? 3600 : 0;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }

  // Apply MQTT settings
  JsonObject mqttSettings = doc["mqtt"];
  if (mqttSettings) {
    strlcpy(mqtt_server, mqttSettings["srv"] | mqttSettings["server"] | "broker.emqx.io", sizeof(mqtt_server));
    mqtt_port = mqttSettings["prt"] | mqttSettings["port"] | 1883;
    strlcpy(mqtt_user, mqttSettings["usr"] | mqttSettings["user"] | "", sizeof(mqtt_user));
    strlcpy(mqtt_password, mqttSettings["pwd"] | mqttSettings["pass"] | "", sizeof(mqtt_password));
    
    client.disconnect();
    client.setServer(mqtt_server, mqtt_port);
  }

  // Apply LED settings
  JsonObject ledSettings = doc["led"];
  if (ledSettings) {
    strlcpy(led_topic, ledSettings["tpc"] | ledSettings["topic"] | "AvantMaker/esp32/led", sizeof(led_topic));
  }
}

void handleSchedules() {
  if (server.method() == HTTP_POST) {
    // Process form data
    for (int i = 0; i < 6; i++) {
      schedules[i].hour = server.arg("h" + String(i)).toInt();
      schedules[i].minute = server.arg("m" + String(i)).toInt();
      schedules[i].state = (server.arg("s" + String(i)) == "on");
      schedules[i].enabled = server.hasArg("e" + String(i));
    }

    scheduleConfigSave(); // Non-blocking save

    server.sendHeader("Location", "/schedules");
    server.send(302, "text/plain", "");
    return;
  }

  String page;
  page.reserve(3000);
  
  page = getPageHeader("LED Schedules");
  page += "<form method='post'>";
  
  for (int i = 0; i < 6; i++) {
    page += "<h3>Schedule " + String(i + 1) + "</h3><div class='grid'>";
    page += "<div class='card'>Time: ";
    page += "<input type='number' name='h" + String(i) + "' min='0' max='23' value='" + String(schedules[i].hour) + "' style='width:60px;display:inline-block;'> : ";
    page += "<input type='number' name='m" + String(i) + "' min='0' max='59' value='" + String(schedules[i].minute) + "' style='width:60px;display:inline-block;'>";
    page += "</div><div class='card'>Action: ";
    page += "<select name='s" + String(i) + "'>";
    page += "<option value='on'" + String(schedules[i].state ? " selected" : "") + ">ON</option>";
    page += "<option value='off'" + String(!schedules[i].state ? " selected" : "") + ">OFF</option>";
    page += "</select></div><div class='card'>Enabled: ";
    page += "<input type='checkbox' name='e" + String(i) + "'" + String(schedules[i].enabled ? " checked" : "") + ">";
    page += "</div></div>";
  }
  
  page += "<button type='submit'>Save Schedules</button></form>";
  page += "<h2>Config Management</h2>";
  page += "<a href='/download_schedule_config'><button>Download Config</button></a>";
  page += getPageFooter();
  
  server.send(200, "text/html", page);
}

void handleDownloadScheduleConfig() {
  DynamicJsonDocument doc(1024);
  JsonArray schedulesArray = doc.to<JsonArray>();
  for (int i = 0; i < 6; i++) {
    JsonObject schedule = schedulesArray.createNestedObject();
    schedule["hour"] = schedules[i].hour;
    schedule["minute"] = schedules[i].minute;
    schedule["state"] = schedules[i].state;
    schedule["enabled"] = schedules[i].enabled;
  }

  String json;
  serializeJson(doc, json);

  server.sendHeader("Content-Disposition", "attachment; filename=schedule_config.json");
  server.send(200, "application/json", json);
}

void handleTimeSettings() {
  if (server.method() == HTTP_POST) {
    gmtOffset_sec = server.arg("gmtOffset").toInt() * 3600;
    use_dst = server.hasArg("dst");
    daylightOffset_sec = use_dst ? 3600 : 0;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    scheduleConfigSave(); // Non-blocking save

    server.sendHeader("Location", "/timesettings");
    server.send(302, "text/plain", "");
    return;
  }

  String page;
  page.reserve(1500);
  
  page = getPageHeader("Time Settings");
  page += "<form method='post'>";
  page += "<label for='gmtOffset'>GMT Offset (hours):</label>";
  page += "<input type='number' id='gmtOffset' name='gmtOffset' value='" + String(gmtOffset_sec / 3600) + "'>";
  page += "<label><input type='checkbox' name='dst'" + String(use_dst ? " checked" : "") + "> Use Daylight Saving Time</label>";
  page += "<button type='submit'>Save Time Settings</button></form>";
  
  page += "<h3>Current Time</h3>";
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 500)) {
    char timeString[30];
    strftime(timeString, sizeof(timeString), "%c", &timeinfo);
    page += "<p>" + String(timeString) + "</p>";
  } else {
    page += "<p>Time not available</p>";
  }
  
  page += getPageFooter();
  server.send(200, "text/html", page);
}

void handleMqttSettings() {
  if (server.method() == HTTP_POST) {
    strlcpy(mqtt_server, server.arg("server").c_str(), sizeof(mqtt_server));
    mqtt_port = server.arg("port").toInt();
    strlcpy(mqtt_user, server.arg("user").c_str(), sizeof(mqtt_user));
    strlcpy(mqtt_password, server.arg("pass").c_str(), sizeof(mqtt_password));
    
    client.disconnect();
    client.setServer(mqtt_server, mqtt_port);

    scheduleConfigSave(); // Non-blocking save

    server.sendHeader("Location", "/mqttsettings");
    server.send(302, "text/plain", "");
    return;
  }

  String page;
  page.reserve(1200);
  
  page = getPageHeader("MQTT Settings");
  page += "<form method='post'>";
  page += "<label for='server'>MQTT Broker:</label>";
  page += "<input type='text' id='server' name='server' value='" + String(mqtt_server) + "'>";
  page += "<label for='port'>Port:</label>";
  page += "<input type='number' id='port' name='port' value='" + String(mqtt_port) + "'>";
  page += "<label for='user'>Username:</label>";
  page += "<input type='text' id='user' name='user' value='" + String(mqtt_user) + "'>";
  page += "<label for='pass'>Password:</label>";
  page += "<input type='password' id='pass' name='pass' value='" + String(mqtt_password) + "'>";
  page += "<button type='submit'>Save MQTT Settings</button></form>";
  page += getPageFooter();
  
  server.send(200, "text/html", page);
}

void handleLedSettings() {
  if (server.method() == HTTP_POST) {
    strlcpy(led_topic, server.arg("topic").c_str(), sizeof(led_topic));
    scheduleConfigSave(); // Non-blocking save

    server.sendHeader("Location", "/ledsettings");
    server.send(302, "text/plain", "");
    return;
  }

  String page;
  page.reserve(800);
  
  page = getPageHeader("LED Settings");
  page += "<form method='post'>";
  page += "<label for='topic'>LED MQTT Topic:</label>";
  page += "<input type='text' id='topic' name='topic' value='" + String(led_topic) + "'>";
  page += "<button type='submit'>Save LED Settings</button></form>";
  page += getPageFooter();
  
  server.send(200, "text/html", page);
}

void handleNotFound() {
  server.send(404, "text/plain", "File Not Found");
}

// --- Setup and Loop ---
void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32 Web Server with LittleFS starting...");

  // Generate unique client ID from MAC address
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  clientId = "AvantLumi-" + mac;
  Serial.print("Using MQTT Client ID: ");
  Serial.println(clientId);
  
  // Initialize schedules with default values
  for (int i = 0; i < 6; i++) {
    schedules[i] = {12, 0, false, false};
  }

  // Initialize LittleFS first
  if (initLittleFS()) {
    loadFullConfig();
  } else {
    Serial.println("LittleFS initialization failed! Using default settings.");
  }

  connectToWiFi();

  // Init time with loaded settings
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // Setup MQTT with loaded configuration
  client.setServer(mqtt_server, mqtt_port);

  // Setup web server routes with optimized handlers
  server.on("/", handleRoot);
  server.on("/download_full_config", handleDownloadFullConfig);
  server.on("/upload_full_config", HTTP_POST, 
    [](){ server.sendHeader("Location", "/"); server.send(302, "text/plain", ""); }, 
    handleUploadFullConfig);
  server.on("/led_on", handleLedOn);
  server.on("/led_off", handleLedOff);
  server.on("/schedules", handleSchedules);
  server.on("/download_schedule_config", handleDownloadScheduleConfig);
  server.on("/timesettings", handleTimeSettings);
  server.on("/mqttsettings", handleMqttSettings);
  server.on("/ledsettings", handleLedSettings);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("System ready - optimized for smooth operation");
}

void loop() {
  // Handle web server requests (highest priority)
  server.handleClient();

  // Non-blocking MQTT connection management
  reconnectMqtt();

  if (client.connected()) {
    client.loop();
  }

  // Process delayed configuration saves
  processConfigSave();

  // Check schedules (once per second)
  static unsigned long lastScheduleCheck = 0;
  if (millis() - lastScheduleCheck > 1000) {
    checkSchedules();
    lastScheduleCheck = millis();
  }

  // Periodically re-sync NTP time
  if (millis() - lastNtpSyncMillis > ntpUpdateIntervalMs) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("NTP time re-synced");
    lastNtpSyncMillis = millis();
  }

  // Small delay to prevent watchdog issues
  delay(1);
}