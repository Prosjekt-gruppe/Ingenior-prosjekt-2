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
#define RST_PIN         39    // RST pin connected to GPIO4
#define SS_PIN          14    // SS (SDA) pin connected to GPIO5
#define MOSI_PIN        16    // MOSI pin connected to GPIO23
#define MISO_PIN        2     // MISO pin connected to GPIO19
#define SCK_PIN         1     // SCK pin connected to GPIO18

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
    // Update MQTT client
    mqtt.update();  // should be called

    // Reconnect if MQTT client is disconnected
    if (!mqtt.isConnected()) {
        connect();
    }

    // Check for new RFID card presence
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    // Card detected
    Serial.println(F("**Card Detected:**"));

    // Dump details about the card
    mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

    // Print Card UID
    Serial.print(F("Card UID:"));
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();

    // Uncomment to see all blocks in hex
    // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

    //-------------------------------------------

    Serial.println(F("\n**End Reading**\n"));

    delay(1000); // Change value if you want to read cards faster

    // Halt PICC and stop encryption on PCD
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    // Publish message to MQTT broker
    static uint32_t prev_ms = millis();
    if (millis() > prev_ms + 1000) {
        prev_ms = millis();
        mqtt.publish("devices/2/data", "world");
    }
}
