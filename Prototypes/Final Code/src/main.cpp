// Include the necessary libraries
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <WebSocketsClient.h>
#include <MQTTPubSubClient.h>
#include <MFRC522.h>
#include <VL53L0X.h>

// Macro definitions for string conversion
#define XSTR(x) #x
#define STR(x) XSTR(x)

// MQTT Configuration (DON'T LEAK THIS TO GITHUB!!!)
const char* MQTT_HOSTNAME = "mqtt.gruppe1.tech"; //Define the MQTT broker address
const char* MQTT_TOPIC = "devices/1/location"; // Define the topic to publish to
const int MQTT_PORT1 = 9002; // Define the port to connect to
const char* MQTT_PASSWORD = STR(MQTT_PASSWD); //The password is defined in a local file
const char* MQTT_USER = STR(MQTT_USERNAME); //The username is defined in a local file

// Wi-Fi Configuration (DON'T LEAK THIS TO GITHUB!!!)
const char* ssid = "";
const char* pass = "";

WebSocketsClient client;
MQTTPubSubClient mqtt;

// Set up the pins for the RFID reader
#define RST_PIN         1
#define SS_PIN          9
#define MOSI_PIN        2
#define MISO_PIN        11
#define SCK_PIN         10
#define BUZZER_PIN      5

// Set up the pins for the distance sensors
#define SDA_PIN 13
#define SCL_PIN 12
#define XSHUT_PIN_1 4
#define XSHUT_PIN_2 6
#define XSHUT_PIN_3 7

// Create VL53L0X sensor instances
VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;

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
    Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C bus
    Serial.begin(115200);

    // Initialize XSHUT pins
    pinMode(XSHUT_PIN_1, OUTPUT);
    pinMode(XSHUT_PIN_2, OUTPUT);
    pinMode(XSHUT_PIN_3, OUTPUT);

    // Shutdown all sensors
    digitalWrite(XSHUT_PIN_1, LOW);
    digitalWrite(XSHUT_PIN_2, LOW);
    digitalWrite(XSHUT_PIN_3, LOW);
    delay(10);  // Short delay to ensure sensors are in shutdown

    // Initialize and assign new address to sensor 1
    digitalWrite(XSHUT_PIN_1, HIGH);
    delay(10);  // Wait for the sensor to boot up
    sensor1.setAddress(0x30);  // New I2C address for sensor 1
    sensor1.init();
    sensor1.startContinuous();

    // Initialize and assign new address to sensor 2
    digitalWrite(XSHUT_PIN_2, HIGH);
    delay(10);  // Wait for the sensor to boot up
    sensor2.setAddress(0x31);  // New I2C address for sensor 2
    sensor2.init();
    sensor2.startContinuous();

    // Initialize and assign new address to sensor 3
    digitalWrite(XSHUT_PIN_3, HIGH);
    delay(10);  // Wait for the sensor to boot up
    sensor3.setAddress(0x32);  // New I2C address for sensor 3
    sensor3.init();
    sensor3.startContinuous();

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

unsigned long TOFMillis = 0;
const unsigned long TOFInterval = 100;
const uint32_t cardReadInterval = 1000; // Interval in milliseconds

void loop() {
    // Update MQTT client regularly
    mqtt.update();

    unsigned long currentMillis = millis();

    // Non-blocking delay for card reading
    static uint32_t lastCardReadTime = 0;

    // Check if it's time to read the card again
    if (currentMillis - lastCardReadTime >= cardReadInterval) {
        // Try to read the card
        if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
            Serial.println(F("\n**Start Reading**"));
            lastCardReadTime = currentMillis;

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
        // No else needed; if no card is present, simply continue
    }

    // Sensor reading code
    if (currentMillis - TOFMillis >= TOFInterval) {
        TOFMillis = currentMillis;

        // Read from sensor 1
        uint16_t distance1 = sensor1.readRangeContinuousMillimeters();
        if (sensor1.timeoutOccurred()) {
            Serial.println("Sensor 1 timeout!");
        } else {
            Serial.print("Distance 1: ");
            Serial.print(distance1);
            Serial.println(" mm");
        }

        // Read from sensor 2
        uint16_t distance2 = sensor2.readRangeContinuousMillimeters();
        if (sensor2.timeoutOccurred()) {
            Serial.println("Sensor 2 timeout!");
        } else {
            Serial.print("Distance 2: ");
            Serial.print(distance2);
            Serial.println(" mm");
        }

        // Read from sensor 3
        uint16_t distance3 = sensor3.readRangeContinuousMillimeters();
        if (sensor3.timeoutOccurred()) {
            Serial.println("Sensor 3 timeout!");
        } else {
            Serial.print("Distance 3: ");
            Serial.print(distance3);
            Serial.println(" mm");
        }
    }
}
