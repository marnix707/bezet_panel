# 📟 Room Schedule Display

A PlatformIO project for ESP32 that fetches and displays a room schedule on an ePaper (CrowPanel) display.

## 🚀 Features

- Connects to WiFi and fetches real-time room bookings via HTTP (Flask backend)
- Displays a time-labeled schedule with booking blocks and titles
- E-paper UI with partial refresh support
- Detects screen changes to avoid unnecessary redraws
- Visual room occupancy indicator (LED + bitmap)
- Time sync from backend
- Modular structure with easy configuration

## 🧰 Tech Stack

- **Board**: ESP32 (CrowPanel)
- **Framework**: Arduino (via PlatformIO)
- **Display**: ePaper (via EPD and EPD_GUI libraries)
- **Networking**: HTTPClient + ArduinoJson
- **Backend**: Flask server serving JSON schedule

## 📂 Project Structure
├── src/ # Main source code
│ ├── main.cpp
│ └── hardware/ # LED, power control
│ └── display/ # GUI rendering, drawing logic
├── lib/ # Custom libraries (if any)
├── include/
│ ├── config_settings.h # WiFi, server, UI settings
│ └── constants.h # Derived constants, timers
├── bitmaps/ # UI images (e.g. bezet/vrij icons)
├── platformio.ini # PlatformIO config
└── README.md


## ⚙️ Configuration

Edit `include/config_settings.h` to customize:

```cpp
const char *ssid = "YourWiFi";
const char *password = "YourPassword";
const String serverIP = "192.168.0.xxx";
const String roomID = "room1";
```
##🔧 Building & Uploading

# Compile the project
pio run

# Upload to the board (check correct port first!)
pio run --target upload --upload-port COM6

