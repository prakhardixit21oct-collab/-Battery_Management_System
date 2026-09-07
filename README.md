# 🔋 Battery Management System (BMS) — SAE ZHCET Effi-Cycle

An ESP32-based real-time Battery Management System built for the **SAE ZHCET Effi-Cycle** vehicle. It continuously monitors pack voltage and current, displays live readings on an onboard LCD, streams telemetry over Bluetooth to a smartphone/laptop, and automatically trips a safety relay if the battery operates outside safe limits.

---

## 🚗 Why this exists

The Effi-Cycle's battery pack is one of the most safety-critical components on the vehicle — overcharging, over-discharging, or drawing excess current can damage the pack or create a fire hazard. This BMS acts as the vehicle's first line of defense: it watches the pack in real time and cuts power the instant something goes out of range, while giving the team live visibility into pack health both on the vehicle and remotely via Bluetooth.

---

## ⚙️ How it works

**1. Voltage sensing**
Battery voltage is stepped down using a resistor-divider network (R1 = 100kΩ, R2 = 5.6kΩ) into the ESP32's ADC range, then scaled back up in software to recover the true pack voltage.

**2. Current sensing**
Pack current is measured using an **ACS758-050B** Hall-effect current sensor (40 mV/A sensitivity), referenced around the ESP32's mid-supply rail (1.65V) to detect current in either direction.

**3. Live local display**
A 16×2 I2C LCD shows real-time voltage and current directly on the vehicle, so the pit crew can check pack status at a glance.

**4. Wireless telemetry**
Every 500 ms, the system broadcasts a CSV-formatted telemetry string — `VOLTAGE,CURRENT,POWER` — over **Bluetooth Serial**, so live pack data can be logged or visualized on a paired phone or laptop without needing a wired connection to the vehicle.

**5. Safety cutoff**
If voltage drops below 42.0V, rises above 54.6V, or current exceeds 35A, the system immediately:
- Cuts power via a relay (`RELAY_PIN`)
- Sends a `ALERT: BMS FAULT TRIP!` message over Bluetooth
- Displays a fault message on the LCD
- Halts further operation until the system is manually reset

---

## 🔩 Hardware Used

| Component | Purpose |
|---|---|
| ESP32 Dev Board | Main controller (ADC + Bluetooth) |
| ACS758-050B | Hall-effect current sensor |
| Resistor divider (100kΩ / 5.6kΩ) | Voltage sensing |
| 16×2 I2C LCD | Local pack status display |
| Relay module | Safety power cutoff |
| Bluetooth (built-in ESP32) | Wireless telemetry |

---

## 🛡️ Safety Thresholds

| Parameter | Limit |
|---|---|
| Minimum Voltage | 42.0 V |
| Maximum Voltage | 54.6 V |
| Maximum Current | 35.0 A |

These values are tuned to the Effi-Cycle's specific battery pack configuration and chemistry.

---

## 📡 Telemetry Format

Data is streamed over Bluetooth Serial every 500ms in a simple CSV format for easy parsing by any receiving app:

```
<voltage>,<current>,<power>
```
Example:
```
48.32,12.75,616.08
```

---

## 🧰 Tech Stack

- **Platform:** ESP32 (Arduino framework)
- **Libraries:** `Wire.h`, `LiquidCrystal_I2C.h`, `BluetoothSerial.h`
- **Language:** C++ (Arduino)

---

## 🚀 Getting Started

1. Wire up the ESP32 with the current sensor, voltage divider, LCD, and relay as per the pin definitions in `BMS_EFFICYCLE.ino`.
2. Open the sketch in the Arduino IDE with ESP32 board support installed.
3. Flash it to the ESP32.
4. Pair a phone/laptop with the Bluetooth device `Efficycle_BMS_Telemetry` to receive live telemetry.

---

## 🔭 Future Improvements

- Log telemetry to an SD card for post-race analysis
- Add per-cell voltage monitoring instead of pack-level only
- Send fault alerts to a companion dashboard app in real time
- Auto-reset logic with a manual override switch instead of a full halt

---

## 👤 Author

**Prakhar Dixit**
Electronics & Communication Engineering (VLSI Design & Technology) — Zakir Husain College of Engineering & Technology, AMU
Recruitment & Workshop Coordinator and contributor, SAE ZHCET
[LinkedIn](https://www.linkedin.com/in/prakhar-dixit-a2bb95377)
