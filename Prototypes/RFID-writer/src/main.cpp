#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <WebSocketsClient.h>  // include before MQTTPubSubClient.h
#include <MQTTPubSubClient.h>
#include <MFRC522.h>

// Macro definitions for string conversion
#define XSTR(x) #x
#define STR(x) XSTR(x)

// MQTT Configuration
const char* MQTT_HOSTNAME = STR(MQTT_HOST);
const char* MQTT_TOPIC = "test/hello"; // Define the topic to publish to
const char* MQTT_PORT1 = STR(MQTT_PORT);
const char* MQTT_PASSWORD = STR(MQTT_PASSWD);
const char* MQTT_USER = STR(MQTT_USERNAME);

// Wi-Fi Configuration
const char* ssid = "NTNU-IOT";
const char* pass = "";

WebSocketsClient client;
MQTTPubSubClient mqtt;

// Pin Definitions for MFRC522 (RFID Reader)
// Set up the pins
#define RST_PIN         39    // RST pin connected to GPIO39
#define SS_PIN          14    // SS (SDA) pin connected to GPIO14
#define MOSI_PIN        16    // MOSI pin connected to GPIO16
#define MISO_PIN        2     // MISO pin connected to GPIO2
#define SCK_PIN         1     // SCK pin connected to GPIO1

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

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
    client.begin(MQTT_HOSTNAME, MQTT_PORT1, "/", "mqtt");  // "mqtt" is required
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
    
    // Begin Wi-Fi connection
    WiFi.begin(ssid, pass);

    // Initialize SPI bus
    SPI.begin();

    // Initialize MFRC522 RFID reader
    mfrc522.PCD_Init();

    // Initialize MQTT client
    mqtt.begin(client);

    // Connect to Wi-Fi, host, and MQTT broker
    connect();

    // Subscribe to MQTT topics (commented out)
    // mqtt.subscribe([](const String& topic, const String& payload, const size_t size) {
    //     Serial.println("mqtt received: " + topic + " - " + payload);
    // });

    // mqtt.subscribe("/hello", [](const String& payload, const size_t size) {
    //     Serial.print("/hello ");
    //     Serial.println(payload);
    // });
}

void loop() {
    // Update MQTT client regularly
    mqtt.update();

    // Reconnect if MQTT client is disconnected
    if (!mqtt.isConnected()) {
        connect();
    }

    // Publish a message to MQTT broker at regular intervals
    static uint32_t prevPublishTime = 0;
    if (millis() - prevPublishTime >= 1000) {
        prevPublishTime = millis();
        mqtt.publish("devices/2/data", "world");
    }

    // Non-blocking delay for card reading
    static uint32_t lastCardReadTime = 0;
    const uint32_t cardReadInterval = 1000; // Interval in milliseconds

    // Check if it's time to read the card again
    if (millis() - lastCardReadTime >= cardReadInterval) {
        // Check for new RFID card presence
        if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
            // Update the last card read time
            lastCardReadTime = millis();

            // Card detected
            Serial.println(F("**Card Detected:**"));

            // Dump details about the card
            mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

            // Setup a string to hold the UID of the card
            String uidString = "";

            // Print Card UID and build string
            Serial.print(F("Card UID:"));
            for (byte i = 0; i < mfrc522.uid.size; i++) {
                // Print each byte to the serial monitor
                Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                Serial.print(mfrc522.uid.uidByte[i], HEX);

                // Add each byte to the string
                if (mfrc522.uid.uidByte[i] < 0x10) {
                    uidString += "0";
                }
                uidString += String(mfrc522.uid.uidByte[i], HEX);
            }
            Serial.println();

            // Convert the UID to uppercase
            uidString.toUpperCase();

            // Now you can use uidString as needed
            // For example, publish it via MQTT
            mqtt.publish("devices/2/data", uidString);

            Serial.println(F("\n**End Reading**\n"));

            // Halt PICC and stop encryption on PCD
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
        }
    }
}
