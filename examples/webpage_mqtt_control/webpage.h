// HTML content for the web interface
const char webpage_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AvantLumi LED Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            background-color: white;
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 30px;
        }
        .control-group {
            margin-bottom: 20px;
            padding: 15px;
            border: 1px solid #ddd;
            border-radius: 5px;
        }
        .control-group h3 {
            margin-top: 0;
            color: #444;
            border-bottom: 1px solid #eee;
            padding-bottom: 10px;
        }
        .control-row {
            display: flex;
            align-items: center;
            margin-bottom: 15px;
        }
        .control-row label {
            flex: 1;
            font-weight: bold;
        }
        .control-row input, .control-row select, .control-row button {
            flex: 2;
            padding: 8px;
            border-radius: 4px;
            border: 1px solid #ddd;
        }
        .control-row button {
            background-color: #4CAF50;
            color: white;
            border: none;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        .control-row button:hover {
            background-color: #45a049;
        }
        .status {
            background-color: #e9f7ef;
            padding: 10px;
            border-radius: 5px;
            margin-top: 20px;
        }
        .mqtt-status {
            display: flex;
            align-items: center;
            margin-bottom: 10px;
        }
        .mqtt-status-indicator {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-right: 10px;
            background-color: #ff0000;
        }
        .mqtt-status-indicator.connected {
            background-color: #00ff00;
        }
        .rgb-inputs {
            display: flex;
            gap: 10px;
        }
        .rgb-inputs input {
            flex: 1;
        }
        .config-buttons {
            display: flex;
            gap: 10px;
        }
        .config-buttons button {
            flex: 1;
        }
        .config-buttons button.save {
            background-color: #2196F3;
        }
        .config-buttons button.load {
            background-color: #FF9800;
        }
        .config-buttons button.save:hover {
            background-color: #0b7dda;
        }
        .config-buttons button.load:hover {
            background-color: #e68a00;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>AvantLumi LED Control</h1>
        
        <div class="control-group">
            <h3>Power Control</h3>
            <div class="control-row">
                <label>Power Switch:</label>
                <select id="switch">
                    <option value="off">OFF</option>
                    <option value="on">ON</option>
                </select>
            </div>
        </div>
        
        <div class="control-group">
            <h3>Brightness & Effects</h3>
            <div class="control-row">
                <label>Brightness (1-5):</label>
                <input type="range" id="bright" min="1" max="5" value="3">
            </div>
            <div class="control-row">
                <label>Fade Effect:</label>
                <select id="fade">
                    <option value="off">OFF</option>
                    <option value="on">ON</option>
                </select>
            </div>
            <div class="control-row">
                <label>Blend Speed (1-5):</label>
                <input type="range" id="blend_spd" min="1" max="5" value="3">
            </div>
        </div>
        
        <div class="control-group">
            <h3>Color Control</h3>
            <div class="control-row">
                <label>RGB Color:</label>
                <div class="rgb-inputs">
                    <input type="number" id="rgb_r" min="0" max="255" placeholder="R">
                    <input type="number" id="rgb_g" min="0" max="255" placeholder="G">
                    <input type="number" id="rgb_b" min="0" max="255" placeholder="B">
                </div>
            </div>
            <div class="control-row">
                <label>Color Name:</label>
                <select id="color">
                    <option value="">Select a color</option>
                    <option value="red">Red</option>
                    <option value="green">Green</option>
                    <option value="blue">Blue</option>
                    <option value="white">White</option>
                    <option value="black">Black (Off)</option>
                    <option value="yellow">Yellow</option>
                    <option value="orange">Orange</option>
                    <option value="pink">Pink</option>
                    <option value="purple">Purple</option>
                    <option value="cyan">Cyan</option>
                    <option value="magenta">Magenta</option>
                    <option value="lime">Lime</option>
                    <option value="maroon">Maroon</option>
                    <option value="olive">Olive</option>
                    <option value="navy">Navy</option>
                    <option value="teal">Teal</option>
                    <option value="silver">Silver</option>
                    <option value="gold">Gold</option>
                    <option value="brown">Brown</option>
                    <option value="coral">Coral</option>
                    <option value="salmon">Salmon</option>
                    <option value="crimson">Crimson</option>
                    <option value="indigo">Indigo</option>
                    <option value="violet">Violet</option>
                    <option value="turquoise">Turquoise</option>
                    <option value="khaki">Khaki</option>
                    <option value="plum">Plum</option>
                    <option value="orchid">Orchid</option>
                    <option value="tan">Tan</option>
                    <option value="hotpink">Hot Pink</option>
                    <option value="forestgreen">Forest Green</option>
                </select>
            </div>
            <div class="control-row">
                <label>Color Palette:</label>
                <select id="palette">
                    <option value="">Select a palette</option>
                    <option value="rainbow">Rainbow</option>
                    <option value="party">Party</option>
                    <option value="ocean">Ocean</option>
                    <option value="heat">Heat</option>
                    <option value="christmas">Christmas</option>
                    <option value="halloween">Halloween</option>
                    <option value="cyberpunk">Cyberpunk</option>
                    <option value="random">Random</option>
                </select>
            </div>
        </div>
        
        <div class="control-group">
            <h3>Configuration</h3>
            <div class="control-row">
                <label>Save/Load Settings:</label>
                <div class="config-buttons">
                    <button class="save" onclick="saveConfig()">Save Config</button>
                    <button class="load" onclick="loadConfig()">Load Config</button>
                </div>
            </div>
        </div>
        
        <div class="status">
            <h3>Status</h3>
            <div class="mqtt-status">
                <div class="mqtt-status-indicator" id="mqtt-indicator"></div>
                <span id="mqtt-status-text">MQTT: Disconnected</span>
            </div>
            <div id="status-content">Loading status...</div>
        </div>
    </div>

    <script>
        // Get status from the ESP32
        function getStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    updateUI(data);
                    document.getElementById('status-content').textContent = JSON.stringify(data, null, 2);
                })
                .catch(error => {
                    console.error('Error getting status:', error);
                    document.getElementById('status-content').textContent = 'Error getting status';
                });
        }
        
        // Update UI with current status
        function updateUI(data) {
            // Update power switch
            document.getElementById('switch').value = data.switch || 'off';
            
            // Update brightness
            document.getElementById('bright').value = data.bright || 3;
            
            // Update fade
            document.getElementById('fade').value = data.fade || 'off';
            
            // Update blend speed
            document.getElementById('blend_spd').value = data.blend_spd || 3;
            
            // Update RGB if available
            if (data.rgb) {
                document.getElementById('rgb_r').value = data.rgb.r || 0;
                document.getElementById('rgb_g').value = data.rgb.g || 0;
                document.getElementById('rgb_b').value = data.rgb.b || 0;
            }
            
            // Update MQTT status
            if (data.mqtt && data.mqtt.connected) {
                document.getElementById('mqtt-indicator').classList.add('connected');
                document.getElementById('mqtt-status-text').textContent = 'MQTT: Connected to ' + (data.mqtt.server || 'unknown');
            } else {
                document.getElementById('mqtt-indicator').classList.remove('connected');
                document.getElementById('mqtt-status-text').textContent = 'MQTT: Disconnected';
            }
        }
        
        // Send control command
        function sendControl(params) {
            let url = '/control?' + params;
            fetch(url)
                .then(response => response.text())
                .then(data => {
                    console.log('Control response:', data);
                    // Refresh status after a short delay
                    setTimeout(getStatus, 500);
                })
                .catch(error => {
                    console.error('Error sending control:', error);
                });
        }
        
        // Save configuration
        function saveConfig() {
            sendControl('config=save');
        }
        
        // Load configuration
        function loadConfig() {
            sendControl('config=load');
        }
        
        // Set up event listeners
        document.addEventListener('DOMContentLoaded', function() {
            // Get initial status
            getStatus();
            
            // Set up periodic status updates
            setInterval(getStatus, 5000);
            
            // Power switch
            document.getElementById('switch').addEventListener('change', function() {
                sendControl('switch=' + this.value);
            });
            
            // Brightness
            document.getElementById('bright').addEventListener('input', function() {
                sendControl('bright=' + this.value);
            });
            
            // Fade
            document.getElementById('fade').addEventListener('change', function() {
                sendControl('fade=' + this.value);
            });
            
            // Blend speed
            document.getElementById('blend_spd').addEventListener('input', function() {
                sendControl('blend_spd=' + this.value);
            });
            
            // RGB inputs
            function updateRGB() {
                const r = document.getElementById('rgb_r').value || 0;
                const g = document.getElementById('rgb_g').value || 0;
                const b = document.getElementById('rgb_b').value || 0;
                sendControl('rgb=' + r + ',' + g + ',' + b);
            }
            
            document.getElementById('rgb_r').addEventListener('input', updateRGB);
            document.getElementById('rgb_g').addEventListener('input', updateRGB);
            document.getElementById('rgb_b').addEventListener('input', updateRGB);
            
            // Color selection
            document.getElementById('color').addEventListener('change', function() {
                if (this.value) {
                    sendControl('color=' + this.value);
                }
            });
            
            // Palette selection
            document.getElementById('palette').addEventListener('change', function() {
                if (this.value) {
                    sendControl('palette=' + this.value);
                }
            });
        });
    </script>
</body>
</html>
)rawliteral";