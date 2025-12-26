# 🚰 Tank Monitoring and Operational Control System (IoT)

An IoT-based smart water tank monitoring and motor control system using **ESP32**, **HC-SR04 Ultrasonic Sensor**, **DHT11**, **MQTT**, and **Node-RED Dashboard**.  
The system supports **automatic motor control**, **manual override**, **real-time monitoring**, and **email alerts** for critical water levels.

---

## 📌 Project Overview

Manual water tank monitoring leads to water overflow, motor dry running, electricity wastage, and lack of real-time visibility.  
This project automates tank monitoring and motor operation using IoT technologies, ensuring efficient and reliable water management.

---

## 🎯 Features

- 📊 Real-time water level monitoring (percentage based)
- 🔁 Automatic motor control using thresholds  
  - Motor **ON** when water level < **10%**
  - Motor **OFF** when water level > **75%**
- 🎛️ Manual motor control via dashboard
- 🌡️ Temperature & humidity monitoring
- 📡 MQTT-based communication
- 🖥️ Node-RED interactive dashboard
- 📧 Email alerts for low and high water levels
- 🌍 Remote monitoring from anywhere

---

## 🧠 System Architecture

**Data Flow:**

Sensors → ESP32 → MQTT Broker → Node-RED Dashboard → User  
                    ↳ Email Alert System  



---

## 🧰 Hardware Components

| Component | Description |
|--------|-------------|
| ESP32 | Main controller with Wi-Fi |
| HC-SR04 | Ultrasonic sensor for water level |
| DHT11 | Temperature & humidity sensor |
| LED / Relay | Motor ON/OFF indication |
| Breadboard | Circuit connections |
| Jumper Wires | Wiring |

---

## 💻 Software & Tools

- **Arduino IDE** – ESP32 programming  
- **MQTT Broker** – `broker.emqx.io`  
- **Node-RED** – Dashboard & alerts  
- **Email Service (Node-RED)** – Alert notifications  
- **GitHub** – Version control  

---

## 🔌 ESP32 Pin Configuration

| Component | GPIO Pin |
|---------|----------|
| Ultrasonic TRIG | GPIO 18 |
| Ultrasonic ECHO | GPIO 19 |
| DHT11 Data | GPIO 14 |
| Motor Relay / Red LED | GPIO 25 |
| Status LED | GPIO 26 |

---

## 📡 MQTT Topics Used

| Purpose | Topic |
|------|------|
| Temperature | `7hills/sensor/temperature` |
| Humidity | `7hills/sensor/humidity` |
| Water Level (%) | `7hills/sensor/distance` |
| Motor Status | `7hills/motor/status` |
| Motor Control | `7hills/LED` |

---

## ⚙️ Working Principle

1. Ultrasonic sensor measures distance from tank top to water surface  
2. Distance is converted to **percentage water level**  
3. ESP32 publishes sensor data to MQTT broker  
4. Node-RED dashboard displays data in real time  
5. Motor operation:
   - Automatic based on thresholds
   - Manual override via dashboard
6. Email alerts are sent during:
   - Low water level
   - High water level

---

## 🖥️ Node-RED Dashboard

Dashboard displays:
- Water level percentage
- Temperature & humidity
- Motor ON/OFF status
- Manual motor control buttons
- Water usage graph
- Location map


---

## 📧 Email Alert System

- Low-level alert email
- High-level alert email
- Includes water level and motor status



---

