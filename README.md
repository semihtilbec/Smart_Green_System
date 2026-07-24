# 🍀ESP32 Smart Green System

**ESP32 Smart Green System** is an automated IoT plant care system. It continuously monitors environmental conditions (air temperature, air humidity, and soil moisture), streams real-time data to **Adafruit IO**, and automatically controls a cooling fan and a water pump based on sensor thresholds.

---

## Features

* **Soil Moisture Monitoring & Auto-Watering:** Calculates soil moisture as a percentage (`%`) using an analog soil moisture sensor and triggers the water pump when the soil gets dry.
* **Environmental Control:** Measures ambient temperature and relative humidity via a DHT22 sensor and operates a fan to control temperature and air circulation.
* **Cloud Integration:** Real-time data streaming and remote status tracking through the Adafruit IO cloud platform.
* **Non-blocking Timing:** Built with `millis()` logic for seamless, non-blocking execution and responsive actuator control.

---

## Hardware Requirements

* **Microcontroller:** ESP32 Development Board
* **Sensors:** 
  * DHT22 (Temperature & Humidity Sensor)
  * Capacitive / Resistive Soil Moisture Sensor
* **Actuators & Relays:**
  * 5V/12V DC Water Pump (Mini Submersible)
  * DC Cooling Fan
* **Connectivity:** Wi-Fi

---

## Required Libraries

* `WiFi.h` (Built-in ESP32 Wi-Fi Library)
* `DHT.h` (Adafruit DHT Sensor Library)
* `AdafruitIO_WiFi.h` (Adafruit IO MQTT/HTTP Library)

---
