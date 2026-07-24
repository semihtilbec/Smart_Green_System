#include <WiFi.h>
#include <DHT.h>
#include "AdafruitIO_WiFi.h"
#include "config.h"

#define DHTPIN 4         
#define DHTTYPE DHT22
#define SOIL_PIN 34       

DHT dht(DHTPIN, DHTTYPE);

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

AdafruitIO_Feed *sicaklikFeed = io.feed("sicaklik");
AdafruitIO_Feed *havaNemFeed  = io.feed("hava-nemi");
AdafruitIO_Feed *toprakNemFeed = io.feed("toprak-nemi");

unsigned long sonVeriZamani = 0;
const long veriAraligi = 10000; 

void setup() {
  Serial.begin(115200);

  dht.begin();

  Serial.print("Wi-Fi'ya baglaniliyor...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nBaglandi!");
  Serial.println("IP: " + WiFi.localIP().toString());
  Serial.println("Gateway: " + WiFi.gatewayIP().toString());

  Serial.print("Adafruit IO'ya baglaniliyor...");
  io.connect();

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nAdafruit IO Baglantisi Basarili!");
}

void loop() {
  io.run();

  if (millis() - sonVeriZamani >= veriAraligi) {
    sonVeriZamani = millis();

    float sicaklik = dht.readTemperature();
    float havaNemi = dht.readHumidity();
    int RawToprakNemi = analogRead(SOIL_PIN);
    
    int toprakNemiYuzde = map(RawToprakNemi, 4095, 1500, 0, 100);
    toprakNemiYuzde = constrain(toprakNemiYuzde, 0, 100);

    if (isnan(sicaklik) || isnan(havaNemi)) {
      Serial.println("DHT22 sensorunden veri okunamadi!");
      return;
    }

    Serial.println("--- Yeni Ölçüm ---");
    Serial.print("Sıcaklık: "); Serial.print(sicaklik); Serial.println(" °C");
    Serial.print("Hava Nemi: %"); Serial.println(havaNemi);
    Serial.print("Toprak Nemi: %"); Serial.println(toprakNemiYuzde);

    sicaklikFeed->save(sicaklik);
    havaNemFeed->save(havaNemi);
    toprakNemFeed->save(toprakNemiYuzde);
  }
}