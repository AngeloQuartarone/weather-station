#include <Arduino.h>
#include "time.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <DHT11.h>
#include <Adafruit_BMP085.h>
#include <ArduinoMqttClient.h>
#include "secrets.h"

#define DHT_SENSOR_PIN 25
#define DFROBOT_PWR_PIN 27
#define uS_TO_S_FACTOR 1000000

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

DHT11 dht11(DHT_SENSOR_PIN);
Adafruit_BMP085 bmp;

const char* ntpServer = "pool.ntp.org";  // NTP server
const long gmtOffset_sec = 0;         // Offset orario per il tuo fuso orario (esempio per GMT+1)
const int daylightOffset_sec = 0;     // Offset per l'ora legale, se applicabile

void setup() {
  Serial.begin(9600);
  int tryCount = 0;



  int pinBuzzer = 33;
  pinMode(pinBuzzer, OUTPUT);
  tone(pinBuzzer, 988,100);
  delay(200);

  // Setup pins
  pinMode(DHT_SENSOR_PIN, INPUT);
  pinMode(DFROBOT_PWR_PIN, OUTPUT);

  // Power up DHT11 sensor
  digitalWrite(DFROBOT_PWR_PIN, HIGH);

  // Setup deep sleep to wake up after 30 seconds
  esp_sleep_enable_timer_wakeup(900 * uS_TO_S_FACTOR);

  // Initialize BMP085 sensor
  tryCount = 0;
  while (!bmp.begin()) {
    tryCount++;
    countCheck(tryCount, 10);
  }

  // Connect to Wi-Fi
  tryCount = 0;
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    tryCount++;
    countCheck(tryCount, 10);
  }
  //Serial.print("WiFi connected with IP: ");
  //Serial.println(WiFi.localIP());


  // Create timestamp
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  char locTime[64];
  tryCount = 0;
  while (!getLocalTime(&timeinfo)) {
    tryCount++;
    countCheck(tryCount, 10);
  }
  strftime(locTime, sizeof(locTime), "%Y-%m-%d %H:%M:%S", &timeinfo);

  // Connect to MQTT broker
  mqttClient.setUsernamePassword(MQTT_USER, MQTT_PASS);
  tryCount = 0;
  while (!mqttClient.connect(BROKER, MQTTPORT)) {
    tryCount++;
    countCheck(tryCount, 10);
  }

  int temperature = dht11.readTemperature();
  int humidity = dht11.readHumidity();
  int pressure = bmp.readPressure();

  // Check sensors errors
  if (temperature == DHT11::ERROR_TIMEOUT || temperature == DHT11::ERROR_CHECKSUM) {
    temperature = -1;
  }
  if (humidity == DHT11::ERROR_TIMEOUT || humidity == DHT11::ERROR_CHECKSUM) {
    humidity = -1;
  }

  // Send data to the server
  String data = "Time:" + String(locTime) + "/T:" + String(temperature) + "/H:" + String(humidity) + "/P:" + String(pressure);
  mqttClient.beginMessage(TOPIC);
  mqttClient.print(data);
  mqttClient.endMessage();
  mqttClient.stop();
  delay(2000);

  // Disconnect and enter deep sleep
  digitalWrite(DFROBOT_PWR_PIN, LOW);
  esp_deep_sleep_start();
}

void countCheck(int tryCount, int n){
  if(tryCount == n){
    digitalWrite(DFROBOT_PWR_PIN, LOW);
    esp_deep_sleep_start();
  }
  else{
    delay(1000); 
  }
  return;
}

void loop() {
  // No code needed here as the device sleeps after setup
}

