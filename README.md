# ESP32 ISS Location-Display

This project fetches real-time orbital data of the International Space Station (ISS) using an ESP32 and the "Where the ISS at?" REST API.

![Demo PNG](demo_new.gif)

## Features
- **Parsing:** Uses `ArduinoJson` to parse the Json response.
- **Time Conversion:** Automatically converts Unix timestamps to human-readable format using the `ctime` library.
- **Robust Networking:** Handles HTTP error codes and WiFi reconnections.
- **Clean Architecture:** Developed using **PlatformIO** with a modular function-based structure.
- **Auto Location Detection:** Auto-location detection via IP-API to notify when ISS is over the user's country using an LED. (N/A if crossing over the ocean.)
- **External Display:** Allows standalone usage.

## Hardware & Tools
- **Microcontroller:** ESP32
- **Framework:** Arduino
- **IDE:** VS Code + PlatformIO
- **Libraries:** `ArduinoJson`, `Adafruit SSD1306`
- **Circuit:** An external LED connected to the GPIO Pin 23 on the ESP32, SSD1306 OLED Screen connected to the pin 22 as SCL and pin 21 as SDA.

## How to Use
1. **Clone the repository:**
   ```bash
   git clone https://github.com/Fwoopr/esp32-iss-location-display.git
   ```
2. **Open it with PlatformIO.** (PlatformIO will automatically handle library dependencies defined in `platformio.ini`.)
3. **Replace the WiFi credentials in `src/main.cpp`.**
4. **Upload to your ESP32 and open the Serial Monitor** (115200 baud).

## 📈 Future Goals
- Integration of an I2C LCD/OLED display for standalone usage.