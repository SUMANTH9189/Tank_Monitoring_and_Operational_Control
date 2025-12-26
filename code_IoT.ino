#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// ====== WiFi Credentials ======
const char* ssid = "SUMANTH";
const char* password = "asdfghjkl";

// ====== MQTT Broker Settings ======
const char* mqtt_server = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqtt_User = "admin";
const char* mqtt_Password = "public";

const char* pubTempTopic = "7hills/sensor/temperature";
const char* pubHumidTopic = "7hills/sensor/humidity";
const char* pubDistanceTopic = "7hills/sensor/distance";
const char* pubMotorStatusTopic = "7hills/motor/status";
const char* subtopic = "7hills/LED";

// ====== GPIO Pin Definitions ======
const int redLED = 25;     // Motor Relay / Status LED
const int yellowLED = 26;
const int DHTPIN = 14;

#define DHTTYPE DHT11

const int trigPin = 18;
const int echoPin = 19;

// ====== Global Variables ======
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastSensorRead = 0;
unsigned long lastUltrasonicRead = 0;
unsigned long lastStatusPrint = 0;

const long sensorInterval = 5000;
const long ultrasonicInterval = 1000;
const long statusPrintInterval = 10000;

bool wifiConnected = false;

// ====== WiFi Connection ======
void setup_wifi() {
  Serial.println("\n[INFO] Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {
    delay(500);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
  } else {
    Serial.println("\n❌ WiFi Failed");
    wifiConnected = false;
  }
}

// ====== MQTT Reconnect ======
void reconnect() {
  while (!client.connected()) {
    Serial.println("[INFO] Connecting to MQTT...");
    String clientId = "ESP32Client-";
    clientId += WiFi.macAddress();

    if (client.connect(clientId.c_str(), mqtt_User, mqtt_Password)) {
      Serial.println("✅ MQTT Connected");
      client.subscribe(subtopic);
    } else {
      Serial.print("❌ MQTT Failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ====== Publish Motor Status ======
void publishMotorStatus(bool state) {
  if (state) {
    client.publish(pubMotorStatusTopic, "ON");
    Serial.println("📤 [PUBLISH] Motor Status: ON");
  } else {
    client.publish(pubMotorStatusTopic, "OFF");
    Serial.println("📤 [PUBLISH] Motor Status: OFF");
  }
}

// ====== LED / Motor Control ======
void controlLED(String msg) {
  if (msg == "on1") {
    digitalWrite(redLED, HIGH);     // Motor ON
    publishMotorStatus(true);
  }
  else if (msg == "off1") {
    digitalWrite(redLED, LOW);      // Motor OFF
    publishMotorStatus(false);
  }
  else if (msg == "on2") {
    digitalWrite(yellowLED, HIGH);
  }
  else if (msg == "off2") {
    digitalWrite(yellowLED, LOW);
  }
}

// ====== MQTT Callback ======
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("📩 [RECEIVED] ");
  Serial.println(msg);

  controlLED(msg);
}

// ====== Publish DHT Sensor Data ======
void publishSensorData() {
  if (millis() - lastSensorRead >= sensorInterval) {
    lastSensorRead = millis();

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (!isnan(temperature) && !isnan(humidity)) {
      char tempString[10], humidString[10];
      dtostrf(temperature, 6, 2, tempString);
      dtostrf(humidity, 6, 2, humidString);

      client.publish(pubTempTopic, tempString);
      client.publish(pubHumidTopic, humidString);

      Serial.print("📤 Temperature: ");
      Serial.println(tempString);
      Serial.print("📤 Humidity: ");
      Serial.println(humidString);
    } else {
      Serial.println("❌ [ERROR] DHT Read Failed!");
    }
  }
}

// ====== Publish Ultrasonic Sensor Data (Percentage) ======
void publishUltrasonicData() {
  if (millis() - lastUltrasonicRead >= ultrasonicInterval) {
    lastUltrasonicRead = millis();

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000);
    float distance = duration * 0.0343 / 2;

    if (duration > 0 && distance <= 24.0) {
      float levelPercent = ((24.0 - distance) / 24.0) * 100.0;
      levelPercent = constrain(levelPercent, 0, 100);

      char percentString[10];
      dtostrf(levelPercent, 6, 2, percentString);

      client.publish(pubDistanceTopic, percentString);

      Serial.print("📤 Water Level: ");
      Serial.print(percentString);
      Serial.println(" %");
    } else {
      Serial.println("❌ Ultrasonic Read Failed!");
    }
  }
}

// ====== Print Status ======
void printStatus() {
  if (millis() - lastStatusPrint >= statusPrintInterval) {
    lastStatusPrint = millis();
    Serial.println("\n===== 📡 STATUS =====");
    Serial.println(WiFi.status() == WL_CONNECTED ? "WiFi: Connected ✅" : "WiFi: Disconnected ❌");
    Serial.println(client.connected() ? "MQTT: Connected ✅" : "MQTT: Disconnected ❌");
    Serial.println("=====================");
  }
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);

  dht.begin();
}

// ====== Loop ======
void loop() {
  if (!wifiConnected || WiFi.status() != WL_CONNECTED) setup_wifi();
  if (!client.connected()) reconnect();

  client.loop();
  publishSensorData();
  publishUltrasonicData();
  printStatus();
}