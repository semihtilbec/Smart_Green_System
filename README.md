# 🌱 ESP32 Smart Green System

ESP32 Smart Green System is an automated IoT plant care system built on the ESP32 microcontroller. It continuously monitors environmental conditions — air temperature, air humidity, and soil moisture — streams real-time data to Adafruit IO, and automatically controls a cooling fan and a water pump based on sensor thresholds. The system also supports full remote manual control via the Adafruit IO dashboard.

---

## ✨ Features

- **Soil Moisture Monitoring & Auto-Watering:** Calculates soil moisture as a percentage using a capacitive soil moisture sensor and triggers the water pump automatically when the soil gets too dry.
- **Environmental Control:** Measures ambient temperature and relative humidity via a DHT22 sensor and activates a cooling fan when temperature exceeds the defined threshold.
- **Dual Control Mode:** Supports both **Automatic Mode** (sensor-based decisions) and **Manual Mode** (remote control via Adafruit IO dashboard toggle).
- **Cloud Integration:** Real-time data streaming and remote status tracking via the Adafruit IO MQTT platform.
- **Non-blocking Timing:** Built with `millis()` logic for seamless, non-blocking execution — the system stays responsive to remote commands at all times.
- **Portable Power:** Powered by a 2S 18650 Li-Ion battery pack with a TP5100 charger and LM2596 buck converter — fully wireless and independent from wall power.

---

## 🔧 Hardware Requirements

| Component | Model / Spec |
|---|---|
| Microcontroller | ESP32 DevKit v1 |
| Temperature & Humidity Sensor | DHT22 |
| Soil Moisture Sensor | Capacitive Soil Moisture v1.2 |
| Water Pump | Mini Submersible 6V |
| Cooling Fan | 5V DC Fan (SUNON 30x30mm) |
| MOSFET Driver | IRLZ44N × 2 |
| Flyback Diode | 1N5819 × 2 |
| Battery | 18650 Li-Ion × 2 (2S Series) |
| Charger Module | TP5100 (Type-C Input) |
| Voltage Regulator | LM2596 Buck Converter (7.4V → 5V) |

---

## 📌 Pin Configuration

| GPIO | Connection | Direction | Protocol |
|---|---|---|---|
| GPIO 4 | DHT22 DATA | Input | OneWire |
| GPIO 34 | Soil Moisture AOUT | Input | ADC1 |
| GPIO 18 | Water Pump MOSFET Gate | Output | Digital |
| GPIO 19 | Cooling Fan MOSFET Gate | Output | Digital |
| VIN | LM2596 5V Output | Input | Power |

---

## 📚 Required Libraries

Install via Arduino IDE → Tools → Manage Libraries:

| Library | Author |
|---|---|
| `DHT sensor library` | Adafruit |
| `Adafruit IO Arduino` | Adafruit |
| `Adafruit GFX Library` | Adafruit |
| `WiFi.h` | Built-in (ESP32) |

---

## ⚙️ Setup & Installation

### 1. Clone the Repository

```bash
git clone https://github.com/kullaniciadin/SmartGreenSystem.git
```

### 2. Create config.h

Create a `config.h` file in the `code/` folder — **do not upload this file to GitHub:**

```cpp
#define IO_USERNAME  "your_adafruit_username"
#define IO_KEY       "your_adafruit_io_key"
#define WIFI_SSID    "your_wifi_ssid"
#define WIFI_PASS    "your_wifi_password"
```

### 3. Create Adafruit IO Feeds

Log in to [Adafruit IO](https://io.adafruit.com) and create the following feeds:

| Feed Name | Type | Description |
|---|---|---|
| `temperature` | Float | Air temperature (°C) |
| `air-humidity` | Float | Relative humidity (%) |
| `soil-moisture` | Integer | Soil moisture (%) |
| `pump-state` | Integer | Pump status (0/1) |
| `fan-state` | Integer | Fan status (0/1) |
| `mode` | String | Control mode (auto/manual) |
| `pump-control` | Integer | Manual pump control (0/1) |
| `fan-control` | Integer | Manual fan control (0/1) |

### 4. Upload the Code

- Board: **ESP32 Dev Module**
- Upload Speed: **921600**
- Select the correct COM/USB port

---

## 🔄 Control Modes

### Manual Mode
Switch to manual mode from the Adafruit IO dashboard by sending `manual` to the `mode` feed. Then use toggle buttons on the dashboard to control the pump and fan directly.

---

## 🎓 Academic Requirements

| Requirement | Implementation |
|---|---|
| Wired Protocol | I2C (OLED SSD1306) |
| Wireless Protocol | Wi-Fi + MQTT |
| Sensor | DHT22 + Capacitive Soil Moisture |
| Actuator | Water Pump + Cooling Fan |
| Cloud Connectivity | Adafruit IO |
| Portable Power | 2S 18650 + TP5100 + LM2596 |
