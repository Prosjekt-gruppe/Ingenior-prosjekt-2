#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <WebSocketsClient.h>  // include before MQTTPubSubClient.h
#include <MQTTPubSubClient.h>


// Macro definitions for string conversion
#define XSTR(x) #x
#define STR(x) XSTR(x)

// MQTT Configuration
const char* MQTT_PASSWORD = STR(MQTT_PASSWD);
const char* MQTT_USER = STR(MQTT_USERNAME);

// Wi-Fi Configuration
const char* ssid = "NTNU-IOT";
const char* pass = "";

WebSocketsClient client;
MQTTPubSubClient mqtt;


// Function to handle Wi-Fi and MQTT connections
void connect() {
connect_to_wifi:
    Serial.print("connecting to wifi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");

connect_to_host:
    Serial.println("connecting to host...");
    client.disconnect();
    client.begin("mqtt.gruppe1.tech", 9002, "/", "mqtt");  // "mqtt" is required
    client.setReconnectInterval(2000);

    Serial.print("connecting to mqtt broker...");
    while (!mqtt.connect("esp32-client", MQTT_USER, MQTT_PASSWORD)) {
        Serial.print(".");
        delay(1000);
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi disconnected");
            goto connect_to_wifi;
        }
        if (!client.isConnected()) {
            Serial.println("WebSocketsClient disconnected");
            goto connect_to_host;
        }
    }
    Serial.println(" connected!");
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("ESP booted up");
    // Begin Wi-Fi connection
    WiFi.begin(ssid, pass);

    // Initialize MQTT client
    mqtt.begin(client);

    // Connect to Wi-Fi, host, and MQTT broker
    connect();

    // Subscribe to MQTT topics (commented out)
    // mqtt.subscribe([](const String& topic, const String& payload, const size_t size) {
    //     Serial.println("mqtt received: " + topic + " - " + payload);
    // });

/*
    mqtt.subscribe("devices/2/data", [](const String& payload, const size_t size) {
         Serial.print("devices/2/data");
         Serial.println(payload);
    });

*/
}

void loop() {
    // Update MQTT client regularly
    mqtt.update();

    // Reconnect if MQTT client is disconnected
    if (!mqtt.isConnected()) {
        connect();
    }

    // Subscribe a message to MQTT broker at regular intervals
    static uint32_t prevPublishTime = 0;
    if (millis() - prevPublishTime >= 1000) {
        prevPublishTime = millis();
         mqtt.subscribe("devices/2/data", [](const String& payload, const size_t size) {
         Serial.print("devices/2/data");
         Serial.println(payload);
    });
    }
    

}