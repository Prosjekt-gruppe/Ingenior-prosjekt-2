#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>         // For Wi-Fi connectivity
#include <PubSubClient.h> // For MQTT connectivity

#define XSTR(x) #x
#define STR(x) XSTR(x)

// Wi-Fi credentials
const char* WIFI_SSID = STR(WIFI_SSID);
const char* WIFI_PASSWD = STR(WIFI_PASSWD);

// MQTT server settings
const char* MQTT_HOSTNAME = "mqtt.gruppe1.tech";
const char* MQTT_USERNAME = STR(MQTT_USERNAME);
const char* MQTT_PASSWD = STR(MQTT_PASSWD);
const char* MQTT_TOPIC = "test/hello"; // Define the topic to publish to

//const char* MQTT_HOST = STR(MQTT_HOST);
const uint32_t PORT_WITHOUT_ENC = (uint32_t)STR(MQTT_PORT);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setupWiFi() {
  delay(10);
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi connected");
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection... ");
    
    // Attempt to connect
    if (mqttClient.connect("ArduinoClient", MQTT_USERNAME, MQTT_PASSWD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  
  // Set MQTT server
  mqttClient.setServer(MQTT_HOSTNAME, MQTT_PORT);
}

void loop() {
  // Ensure Wi-Fi is connected
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
  
  // Ensure MQTT is connected
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  // Publish message to the MQTT topic
  mqttClient.loop(); // Handle MQTT client events
  mqttClient.publish(MQTT_TOPIC, "Hello World!");
  Serial.println("Message 'Hello World!' sent via MQTT");

  delay(1000); // Wait 1 second before sending the next message
}
