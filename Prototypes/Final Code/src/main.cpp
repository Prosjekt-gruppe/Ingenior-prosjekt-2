// Include the necessary libraries
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <WebSocketsClient.h>
#include <MQTTPubSubClient.h>
#include <MFRC522.h>

// Macro definitions for string conversion
#define XSTR(x) #x
#define STR(x) XSTR(x)

// MQTT Configuration
const char* MQTT_HOSTNAME = "mqtt.gruppe1.tech";
const char* MQTT_TOPIC = "devices/1/location"; // Define the topic to publish to
const int MQTT_PORT1 = 9002;
const char* MQTT_PASSWORD = STR(MQTT_PASSWD);
const char* MQTT_USER = STR(MQTT_USERNAME);

// Wi-Fi Configuration
const char* ssid = "NTNU-IOT";
const char* pass = "";

WebSocketsClient client;
MQTTPubSubClient mqtt;

// Set up the pins
#define RST_PIN         13    // RST pin connected to GPIO39
#define SS_PIN          9    // SS (SDA) pin connected to GPIO14
#define MOSI_PIN        11    // MOSI pin connected to GPIO16
#define MISO_PIN        12     // MISO pin connected to GPIO2
#define SCK_PIN         10     // SCK pin connected to GPIO1
#define BUZZER_PIN      5

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

    pinMode(BUZZER_PIN, OUTPUT);

    tone(BUZZER_PIN, 740, 400);
    tone(BUZZER_PIN, 587, 400);
    tone(BUZZER_PIN, 440, 400);
    tone(BUZZER_PIN, 587, 400);
    tone(BUZZER_PIN, 659, 400);
    tone(BUZZER_PIN, 880, 800);
    tone(BUZZER_PIN, 659, 400);
    tone(BUZZER_PIN, 740, 400);
    tone(BUZZER_PIN, 659, 400);
    tone(BUZZER_PIN, 440, 400);
    tone(BUZZER_PIN, 587, 800);
    
    // Begin Wi-Fi connection
    WiFi.begin(ssid, pass);

    // Initialize SPI bus
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

    // Initialize MFRC522 RFID reader
    mfrc522.PCD_Init();

    // Initialize MQTT client
    mqtt.begin(client);

    // Connect to Wi-Fi, host, and MQTT broker
    connect();
}

void loop() {
    // Update MQTT client regularly
    mqtt.update();

    // Non-blocking delay for card reading
    static uint32_t lastCardReadTime = 0;
    const uint32_t cardReadInterval = 1000; // Interval in milliseconds

    // Check if it's time to read the card again
    if (millis() - lastCardReadTime >= cardReadInterval) {
        // Check for new RFID card presence
        if ( ! mfrc522.PICC_IsNewCardPresent()){
            return;
        }
        if ( ! mfrc522.PICC_ReadCardSerial()){
            return;
        }
        Serial.println(F("\n**Start Reading**"));
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
        mqtt.publish(MQTT_TOPIC, uidString);

        Serial.println(F("\n**End Reading**\n"));

        // Halt PICC and stop encryption on PCD
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
    }
}