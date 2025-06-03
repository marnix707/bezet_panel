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

