#ifndef WEBPAGE_H
#define WEBPAGE_H

// HTML content for the web interface
const char webpage_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>AvantLumi Control Panel</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f4f4f4;
      color: #333;
      margin: 0;
      padding: 20px;
    }
    .container {
      max-width: 800px;
      margin: 0 auto;
      background-color: #fff;
      padding: 20px;
      border-radius: 5px;
      box-shadow: 0 2px 5px rgba(0,0,0,0.1);
    }
    h1 {
      color: #0056b3;
      text-align: center;
    }
    .control-group {
      margin-bottom: 20px;
      padding: 15px;
      border: 1px solid #ddd;
      border-radius: 5px;
    }
    .control-group h3 {
      margin-top: 0;
      color: #0056b3;
    }
    .switch {
      position: relative;
      display: inline-block;
      width: 60px;
      height: 34px;
    }
    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }
    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: .4s;
      border-radius: 34px;
    }
    .slider:before {
      position: absolute;
      content: "";
      height: 26px;
      width: 26px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      transition: .4s;
      border-radius: 50%;
    }
    input:checked + .slider {
      background-color: #0056b3;
    }
    input:checked + .slider:before {
      transform: translateX(26px);
    }
    .range-slider {
      width: 100%;
      margin: 10px 0;
    }
    .rgb-inputs {
      display: flex;
      gap: 10px;
      margin-bottom: 10px;
    }
    .rgb-inputs input {
      flex: 1;
      padding: 8px;
      border: 1px solid #ddd;
      border-radius: 4px;
    }
    select {
      width: 100%;
      padding: 8px;
      margin-bottom: 10px;
      border: 1px solid #ddd;
      border-radius: 4px;
    }
    button {
      background-color: #0056b3;
      color: white;
      border: none;
      padding: 10px 15px;
      border-radius: 4px;
      cursor: pointer;
      font-size: 16px;
    }
    button:hover {
      background-color: #004494;
    }
    .config-buttons {
      display: flex;
      gap: 10px;
      margin-top: 10px;
    }
    .config-buttons button {
      flex: 1;
    }
    .status {
      text-align: center;
      margin-top: 20px;
      padding: 10px;
      background-color: #e9f7ef;
      border-radius: 4px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>AvantLumi Control Panel</h1>
    
    <div class="control-group">
      <h3>Power Control</h3>
      <label class="switch">
        <input type="checkbox" id="switch" onchange="updateControl()">
        <span class="slider"></span>
      </label>
      <span id="switch-status">OFF</span>
    </div>
    
    <div class="control-group">
      <h3>Brightness</h3>
      <input type="range" min="1" max="5" value="3" class="range-slider" id="brightness" onchange="updateControl()">
      <span id="brightness-value">3</span>
    </div>
    
    <div class="control-group">
      <h3>Fade Effect</h3>
      <label class="switch">
        <input type="checkbox" id="fade" onchange="updateControl()">
        <span class="slider"></span>
      </label>
      <span id="fade-status">OFF</span>
    </div>
    
    <div class="control-group">
      <h3>RGB Color</h3>
      <div class="rgb-inputs">
        <input type="number" id="red" min="0" max="255" value="255" placeholder="Red">
        <input type="number" id="green" min="0" max="255" value="0" placeholder="Green">
        <input type="number" id="blue" min="0" max="255" value="0" placeholder="Blue">
      </div>
      <button onclick="applyRGB()">Apply RGB</button>
    </div>
    
    <div class="control-group">
      <h3>Color Name</h3>
      <select id="color-name">
        <option value="red">Red</option>
        <option value="green">Green</option>
        <option value="blue">Blue</option>
        <option value="yellow">Yellow</option>
        <option value="orange">Orange</option>
        <option value="purple">Purple</option>
        <option value="pink">Pink</option>
        <option value="white">White</option>
        <option value="cyan">Cyan</option>
        <option value="magenta">Magenta</option>
      </select>
      <button onclick="applyColor()">Apply Color</button>
    </div>
    
    <div class="control-group">
      <h3>Color Palette</h3>
      <select id="palette">
        <option value="rainbow">Rainbow</option>
        <option value="party">Party</option>
        <option value="ocean">Ocean</option>
        <option value="heat">Heat</option>
        <option value="christmas">Christmas</option>
        <option value="halloween">Halloween</option>
        <option value="cyberpunk">Cyberpunk</option>
        <option value="random">Random</option>
      </select>
      <button onclick="applyPalette()">Apply Palette</button>
    </div>
    
    <div class="control-group">
      <h3>Blend Speed</h3>
      <input type="range" min="1" max="5" value="3" class="range-slider" id="blend-speed" onchange="updateControl()">
      <span id="blend-speed-value">3</span>
    </div>
    
    <div class="control-group">
      <h3>Configuration</h3>
      <div class="config-buttons">
        <button onclick="saveConfig()">Save Config</button>
        <button onclick="loadConfig()">Load Config</button>
      </div>
    </div>
    
    <div class="status" id="status">
      Ready
    </div>
  </div>

  <script>
    // Update control values when sliders or switches change
    function updateControl() {
      document.getElementById('brightness-value').textContent = document.getElementById('brightness').value;
      document.getElementById('blend-speed-value').textContent = document.getElementById('blend-speed').value;
      document.getElementById('switch-status').textContent = document.getElementById('switch').checked ? 'ON' : 'OFF';
      document.getElementById('fade-status').textContent = document.getElementById('fade').checked ? 'ON' : 'OFF';
      
      // Send update to ESP32
      const switchState = document.getElementById('switch').checked ? 'on' : 'off';
      const brightness = document.getElementById('brightness').value;
      const fadeState = document.getElementById('fade').checked ? 'on' : 'off';
      const blendSpeed = document.getElementById('blend-speed').value;
      
      fetch(`/control?switch=${switchState}&bright=${brightness}&fade=${fadeState}&blend_spd=${blendSpeed}`)
        .then(response => response.text())
        .then(data => {
          document.getElementById('status').textContent = data;
        })
        .catch(error => {
          document.getElementById('status').textContent = 'Error: ' + error;
        });
    }
    
    // Apply RGB color
    function applyRGB() {
      const red = document.getElementById('red').value;
      const green = document.getElementById('green').value;
      const blue = document.getElementById('blue').value;
      
      fetch(`/control?rgb=${red},${green},${blue}`)
        .then(response => response.text())
        .then(data => {
          document.getElementById('status').textContent = data;
        })
        .catch(error => {
          document.getElementById('status').textContent = 'Error: ' + error;
        });
    }
    
    // Apply color by name
    function applyColor() {
      const colorName = document.getElementById('color-name').value;
      
      fetch(`/control?color=${colorName}`)
        .then(response => response.text())
        .then(data => {
          document.getElementById('status').textContent = data;
        })
        .catch(error => {
          document.getElementById('status').textContent = 'Error: ' + error;
        });
    }
    
    // Apply color palette
    function applyPalette() {
      const palette = document.getElementById('palette').value;
      
      fetch(`/control?palette=${palette}`)
        .then(response => response.text())
        .then(data => {
          document.getElementById('status').textContent = data;
        })
        .catch(error => {
          document.getElementById('status').textContent = 'Error: ' + error;
        });
    }
    
    // Save configuration
    function saveConfig() {
      fetch('/control?config=save')
        .then(response => response.text())
        .then(data => {
          document.getElementById('status').textContent = data;
        })
        .catch(error => {
          document.getElementById('status').textContent = 'Error: ' + error;
        });
    }
    
    // Load configuration
    function loadConfig() {
      fetch('/control?config=load')
        .then(response => response.text())
        .then(data => {
          document.getElementById('status').textContent = data;
        })
        .catch(error => {
          document.getElementById('status').textContent = 'Error: ' + error;
        });
    }
    
    // Initialize the page with current status
    window.onload = function() {
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          if (data.switch === 'on') {
            document.getElementById('switch').checked = true;
          }
          document.getElementById('brightness').value = data.bright || 3;
          document.getElementById('brightness-value').textContent = data.bright || 3;
          
          if (data.fade === 'on') {
            document.getElementById('fade').checked = true;
          }
          
          document.getElementById('blend-speed').value = data.blend_spd || 3;
          document.getElementById('blend-speed-value').textContent = data.blend_spd || 3;
          
          // Update status text
          document.getElementById('switch-status').textContent = data.switch === 'on' ? 'ON' : 'OFF';
          document.getElementById('fade-status').textContent = data.fade === 'on' ? 'ON' : 'OFF';
        })
        .catch(error => {
          document.getElementById('status').textContent = 'Error loading status: ' + error;
        });
    };
  </script>
</body>
</html>
)rawliteral";

#endif // WEBPAGE_H