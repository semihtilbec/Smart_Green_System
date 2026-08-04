#include <WiFi.h>
#include <DHT.h>
#include "AdafruitIO_WiFi.h"
#include "config.h"

#define DHTPIN        4
#define DHTTYPE       DHT22
#define SOIL_PIN      34
#define RELAY_PUMP    18
#define RELAY_FAN     19

#define TEMP_THRESHOLD  30.0
#define SOIL_THRESHOLD  30

#define RELAY_ON   LOW
#define RELAY_OFF  HIGH

DHT dht(DHTPIN, DHTTYPE);
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

AdafruitIO_Feed *temperatureFeed   = io.feed("temperature");
AdafruitIO_Feed *airHumidityFeed   = io.feed("air-humidity");
AdafruitIO_Feed *soilMoistureFeed  = io.feed("soil-moisture");
AdafruitIO_Feed *pumpStateFeed     = io.feed("pump-state");
AdafruitIO_Feed *fanStateFeed      = io.feed("fan-state");

AdafruitIO_Feed *modeFeed          = io.feed("mode"); 
AdafruitIO_Feed *pumpControlFeed   = io.feed("pump-control");
AdafruitIO_Feed *fanControlFeed    = io.feed("fan-control");

bool isAutoMode   = true;  
bool manualPump   = false;
bool manualFan    = false;

unsigned long lastDataTime = 0;
const long dataInterval = 10000;

void handleMode(AdafruitIO_Data *data) {
  String mode = data->toString();
  mode.toLowerCase();
  if (mode == "auto") {
    isAutoMode = true;
    Serial.println("[MODE] Otomatik mod aktif.");
  } else if (mode == "manual") {
    isAutoMode = false;
    Serial.println("[MODE] Manuel mod aktif.");
  }
}

void handlePumpControl(AdafruitIO_Data *data) {
  if (!isAutoMode) {
    manualPump = data->toInt() == 1;
    digitalWrite(RELAY_PUMP, manualPump ? RELAY_ON : RELAY_OFF);
    Serial.print("[MANUAL] Pompa: ");
    Serial.println(manualPump ? "AÇIK" : "KAPALI");
  }
}

void handleFanControl(AdafruitIO_Data *data) {
  if (!isAutoMode) {
    manualFan = data->toInt() == 1;
    digitalWrite(RELAY_FAN, manualFan ? RELAY_ON : RELAY_OFF);
    Serial.print("[MANUAL] Fan: ");
    Serial.println(manualFan ? "AÇIK" : "KAPALI");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PUMP, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  digitalWrite(RELAY_PUMP, RELAY_OFF);
  digitalWrite(RELAY_FAN, RELAY_OFF);

  dht.begin();

  Serial.print("Wi-Fi bağlanılıyor...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Bağlandı!");

  Serial.print("Adafruit IO bağlanılıyor...");
  io.connect();
  while (io.status() < AIO_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nAdafruit IO Bağlandı!");

  modeFeed->onMessage(handleMode);
  pumpControlFeed->onMessage(handlePumpControl);
  fanControlFeed->onMessage(handleFanControl);

  modeFeed->get();
  pumpControlFeed->get();
  fanControlFeed->get();
}

void loop() {
  io.run();

  if (millis() - lastDataTime >= dataInterval) {
    lastDataTime = millis();

    float temperature     = dht.readTemperature();
    float airHumidity     = dht.readHumidity();
    int   rawSoil         = analogRead(SOIL_PIN);
    int   soilPercent     = map(rawSoil, 4095, 1500, 0, 100);
    soilPercent           = constrain(soilPercent, 0, 100);

    if (isnan(temperature) || isnan(airHumidity)) {
      Serial.println("[HATA] DHT22 okunamadi!");
      return;
    }

    if (isAutoMode) {
      if (temperature >= TEMP_THRESHOLD) {
        digitalWrite(RELAY_FAN, RELAY_ON);
        Serial.println("[AUTO] Yüksek sıcaklık! Fan AÇILDI.");
      } else {
        digitalWrite(RELAY_FAN, RELAY_OFF);
        Serial.println("[AUTO] Normal sıcaklık. Fan KAPALI.");
      }

      if (soilPercent <= SOIL_THRESHOLD) {
        digitalWrite(RELAY_PUMP, RELAY_ON);
        Serial.println("[AUTO] Kuru toprak! Pompa AÇILDI.");
        delay(3000);
        digitalWrite(RELAY_PUMP, RELAY_OFF);
        Serial.println("[AUTO] Pompa KAPATILDI.");
      } else {
        digitalWrite(RELAY_PUMP, RELAY_OFF);
        Serial.println("[AUTO] Toprak nemli. Pompa KAPALI.");
      }
    }

    else {
      Serial.println("[MANUAL] Mod aktif — kontrol Adafruit IO'dan yapılıyor.");
    }

    Serial.println("─────────────────────────────");
    Serial.print("Mod:           "); Serial.println(isAutoMode ? "OTOMATIK" : "MANUEL");
    Serial.print("Sicaklik:      "); Serial.print(temperature); Serial.println(" C");
    Serial.print("Hava Nemi:     %"); Serial.println(airHumidity);
    Serial.print("Toprak Nemi:   %"); Serial.println(soilPercent);
    Serial.println("─────────────────────────────");

    temperatureFeed->save(temperature);
    airHumidityFeed->save(airHumidity);
    soilMoistureFeed->save(soilPercent);
    fanStateFeed->save(digitalRead(RELAY_FAN) == RELAY_ON ? 1 : 0);
    pumpStateFeed->save(digitalRead(RELAY_PUMP) == RELAY_ON ? 1 : 0);
  }
}
