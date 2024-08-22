#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT11.h>
#include <Adafruit_BMP085.h>
#include <String.h>
#include <Wire.h>
#include "secrets.h"

const uint16_t port = 8080;
const char* host = "192.168.1.33";

#define DHT_SENSOR_TYPE DHT_TYPE_11
#define DHT_SENSOR_PIN 25
#define DFROBOT_PWR_PIN 27
#define uS_TO_S_FACTOR 1000000


hw_timer_t* timer = NULL;
volatile byte dfrobotState = HIGH;
unsigned long previousMillis = 0;
const long intervalOFF = 480000;  //dfrobot solar power manager off time (8 min)
const long intervalON = 120000;   //dfrobot solar power manager on time (2 min)
int temperature = 0, humidity = 0, pressure = 0;
int temperature_old = 0, humidity_old = 0;
DHT11 dht11(DHT_SENSOR_PIN);
Adafruit_BMP085 bmp;

void IRAM_ATTR onTimer();



void setup() {

  Serial.begin(9600);
  pinMode(DHT_SENSOR_PIN, INPUT);
  pinMode(DFROBOT_PWR_PIN, OUTPUT);

  digitalWrite(DFROBOT_PWR_PIN, HIGH);
  esp_sleep_enable_timer_wakeup(300 * uS_TO_S_FACTOR);
  WiFiClient client;

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, onTimer, true);
  timerAlarmWrite(timer, 120 * uS_TO_S_FACTOR, true);
  timerWrite(timer, 0);

  delay(1000);



  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }


  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    timerAlarmEnable(timer);
    delay(500);
    Serial.println("...");
  }

  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  while (!client.connect(host, port)) {
    timerAlarmEnable(timer);
    Serial.println("Connection to host failed");
    delay(500);
  }

  Serial.println("Connected to server successful!");
  temperature = dht11.readTemperature();
  humidity = dht11.readHumidity();


  if (temperature == DHT11::ERROR_TIMEOUT || temperature == DHT11::ERROR_CHECKSUM) {
    temperature = -1;
  }
  
  
  if (humidity == DHT11::ERROR_TIMEOUT || humidity == DHT11::ERROR_CHECKSUM) {
    humidity = -1;
  }


  pressure = bmp.readPressure();

  String x = "T:" + String(temperature) + "/H:" + String(humidity) + "/P:" + String(pressure);

  client.print(x);

  String c = client.readStringUntil('\0');
  Serial.println(c);

  Serial.println("Disconnecting...");
  client.stop();


  digitalWrite(DFROBOT_PWR_PIN, LOW);
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
}


void IRAM_ATTR onTimer() {
  esp_deep_sleep_start();
}