#include <WiFi.h>
#include <DHT.h>
#include "AdafruitIO_WiFi.h"
#include "config.h"

#define DHTPIN 4
#define DHTTYPE DHT22
#define SOIL_PIN 34

#define RELAY_PUMP 18
#define RELAY_FAN  19

#define TEMP_THRESHOLD 30.0
#define SOIL_THRESHOLD 30

#define RELAY_ON  LOW
#define RELAY_OFF HIGH

DHT dht(DHTPIN, DHTTYPE);

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

AdafruitIO_Feed *temperatureFeed = io.feed("temperature");
AdafruitIO_Feed *airHumidityFeed  = io.feed("air-humidity");
AdafruitIO_Feed *soilMoistureFeed = io.feed("soil-moisture");
AdafruitIO_Feed *pumpStateFeed    = io.feed("pump-state");
AdafruitIO_Feed *fanStateFeed     = io.feed("fan-state");

unsigned long lastDataTime = 0;
const long dataInterval = 10000;

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PUMP, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  digitalWrite(RELAY_PUMP, RELAY_OFF);
  digitalWrite(RELAY_FAN, RELAY_OFF);

  dht.begin();

  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  Serial.print("Connecting to Adafruit IO...");
  io.connect();

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nAdafruit IO Connected!");
}

void loop() {
  io.run();

  if (millis() - lastDataTime >= dataInterval) {
    lastDataTime = millis();

    float temperature = dht.readTemperature();
    float airHumidity = dht.readHumidity();
    int rawSoilMoisture = analogRead(SOIL_PIN);
    
    int soilMoisturePercent = map(rawSoilMoisture, 4095, 1500, 0, 100);
    soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

    if (isnan(temperature) || isnan(airHumidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    bool isFanOn = false;
    if (temperature >= TEMP_THRESHOLD) {
      digitalWrite(RELAY_FAN, RELAY_ON);
      isFanOn = true;
      Serial.println("-> High Temperature! FAN ACTIVATED.");
    } else {
      digitalWrite(RELAY_FAN, RELAY_OFF);
      Serial.println("-> Normal Temperature. FAN OFF.");
    }

    bool isPumpOn = false;
    if (soilMoisturePercent <= SOIL_THRESHOLD) {
      digitalWrite(RELAY_PUMP, RELAY_ON);
      isPumpOn = true;
      Serial.println("-> Dry Soil! PUMP ACTIVATED.");
    } else {
      digitalWrite(RELAY_PUMP, RELAY_OFF);
      Serial.println("-> Moist Soil. PUMP OFF.");
    }

    Serial.println("--- New Measurement ---");
    Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Air Humidity: %"); Serial.println(airHumidity);
    Serial.print("Soil Moisture: %"); Serial.println(soilMoisturePercent);

    temperatureFeed->save(temperature);
    airHumidityFeed->save(airHumidity);
    soilMoistureFeed->save(soilMoisturePercent);
    fanStateFeed->save(isFanOn ? 1 : 0);
    pumpStateFeed->save(isPumpOn ? 1 : 0);
  }
}