#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>  // include before MQTTPubSubClient.h
#include <MQTTPubSubClient.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

const char* MQTT_HOSTNAME = "mqtt.gruppe1.tech";
const char* MQTT_USERNAME = STR(MQTT_USERNAME);
const char* MQTT_PASSWD = STR(MQTT_PASSWD);
const char* MQTT_TOPIC = "test/hello"; // Define the topic to publish to

//const char* MQTT_HOST = STR(MQTT_HOST);
const uint32_t PORT_WITHOUT_ENC = (uint32_t)STR(MQTT_PORT);

const char* ssid = "NTNU-IOT";
const char* pass = "";

WebSocketsClient client;
MQTTPubSubClient mqtt;

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
    client.begin("test.mosquitto.org", 8080, "/", "mqtt");  // "mqtt" is required
    client.setReconnectInterval(2000);

    Serial.print("connecting to mqtt broker...");
    while (!mqtt.connect("arduino", "public", "public")) {
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
    Serial.begin(115200);
    WiFi.begin(ssid, pass);

    // initialize mqtt client
    mqtt.begin(client);

    // connect to wifi, host and mqtt broker
    connect();

    // subscribe callback which is called when every packet has come
    mqtt.subscribe([](const String& topic, const String& payload, const size_t size) {
        Serial.println("mqtt received: " + topic + " - " + payload);
    });

    // subscribe topic and callback which is called when /hello has come
    mqtt.subscribe("/hello", [](const String& payload, const size_t size) {
        Serial.print("/hello ");
        Serial.println(payload);
    });
}

void loop() {
    mqtt.update();  // should be called

    if (!mqtt.isConnected()) {
        connect();
    }

    // publish message
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 1000) {
        prev_ms = millis();
        mqtt.publish("/hello", "world");
    }
}