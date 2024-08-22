#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <DHT11.h>
#include <Adafruit_BMP085.h>
#include "secrets.h"

const uint16_t port = 8080;
const char* host = "192.168.1.33";

#define DHT_SENSOR_PIN 25
#define DFROBOT_PWR_PIN 27
#define uS_TO_S_FACTOR 1000000

DHT11 dht11(DHT_SENSOR_PIN);
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  
  // Setup pins
  pinMode(DHT_SENSOR_PIN, INPUT);
  pinMode(DFROBOT_PWR_PIN, OUTPUT);

  // Initialize the DHT11 sensor
  digitalWrite(DFROBOT_PWR_PIN, HIGH);

  // Setup deep sleep to wake up after 300 seconds (5 minutes)
  esp_sleep_enable_timer_wakeup(300 * uS_TO_S_FACTOR);

  // Initialize BMP085 sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    digitalWrite(DFROBOT_PWR_PIN, LOW);
    esp_deep_sleep_start();
  }

  // Connect to Wi-Fi
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  // Connect to the server
  WiFiClient client;
  while (!client.connect(host, port)) {
    Serial.println("Connection to host failed");
    delay(500);
  }
  Serial.println("Connected to server successfully!");

  // Read sensor data
  int temperature = dht11.readTemperature();
  int humidity = dht11.readHumidity();
  int pressure = bmp.readPressure();

  // Check for sensor errors
  if (temperature == DHT11::ERROR_TIMEOUT || temperature == DHT11::ERROR_CHECKSUM) {
    temperature = -1;
  }
  if (humidity == DHT11::ERROR_TIMEOUT || humidity == DHT11::ERROR_CHECKSUM) {
    humidity = -1;
  }

  // Send data to the server
  String data = "T:" + String(temperature) + "/H:" + String(humidity) + "/P:" + String(pressure);
  client.print(data);
  
  // Print server response
  String response = client.readStringUntil('\0');
  Serial.println(response);

  // Disconnect and enter deep sleep
  Serial.println("Disconnecting...");
  client.stop();
  digitalWrite(DFROBOT_PWR_PIN, LOW);
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
  // No code needed here as the device sleeps after setup
}
