# weather-station

This repository contains the code for a simple weather station using an ESP32 board. The station collects temperature, humidity, and pressure data and sends it to a remote server. The system is designed to operate efficiently with minimal power consumption, making it suitable for solar-powered applications.

### Features
- **Temperature & Humidity**: Uses a DHT11 sensor to measure temperature and humidity
- **Pressure**: Uses an Adafruit BMP085 sensor to measure atmospheric pressure
- **Wi-Fi Communication**: Connects to a Wi-Fi network and sends data to a specified server
- **Power Management**: Integrates with a solar power manager and enters deep sleep mode to conserve energy
- **Server Integration**: The project is currently configured to communicate with a server that processes all received data and makes it available to the user via a Telegram bot

### Hardware Requirements
- Arduino board or similar
- DHT11 Sensor: For temperature and humidity measurements
- Adafruit BMP085 Sensor: For pressure measurements
- DFRobot Solar Power Manager: (optional) for efficient energy management
- Wi-Fi Module: Built-in or external, depending on your Arduino board

### Setup
1. Hardware Connections:
    - Connect the DHT11 sensor to pin 25
    - Connect the BMP085 sensor to the I2C bus (usually SDA and SCL pins)
    - Connect the solar power manager control pin to pin 27
2. Wi-Fi Configuration:
    - Replace `SECRET_SSID` and `SECRET_PASS` in `secrets.h` with your Wi-Fi credentials
3. Server Configuration:
    - Update the `host` and `port` variables in the code with your server’s IP address and port
4. Power Management:
    - The system is configured to enter deep sleep for 5 minutes (`300 seconds`) after each data transmission. You can adjust the sleep interval by modifying the value in `esp_sleep_enable_timer_wakeup(300 * uS_TO_S_FACTOR);`
5. Compile and Upload:
    - Use the Arduino IDE or similar to compile and upload the code to your board

### How It Works
1. On startup, the Arduino connects to the specified Wi-Fi network
2. It then attempts to connect to the remote server using the provided IP address and port
3. The temperature, humidity, and pressure are measured using the DHT11 and BMP085 sensors
4. The data is formatted into a string and sent to the server
5. The Arduino enters deep sleep mode for 5 minutes to conserve power; the cycle repeats upon wakeup

### Troubleshooting
- Wi-Fi Connection Issues: Ensure that the Wi-Fi credentials are correct and that the Arduino is within range of the Wi-Fi router
- Sensor Issues: Double-check the wiring if the sensor data returns errors. If the BMP085 sensor is not found, the system will immediately enter deep sleep
- Power Issues: If using the solar power manager, ensure that it’s properly connected and receiving sufficient light

### Future Improvements
- Implement more robust error handling for sensor data
- Add support for additional sensors (e.g., wind speed, rainfall)
- **Transition to MQTT Protocol**: Migrate from the current communication protocol to an MQTT-based architecture for more efficient data transmission, better scalability, and improved integration with IoT platforms


