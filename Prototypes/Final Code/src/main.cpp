// Include the necessary libraries
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <WebSocketsClient.h>
#include <MQTTPubSubClient.h>
#include <MFRC522.h>
#include <VL53L0X.h>

// Macro definitions for string conversion
#define XSTR(x) #x
#define STR(x) XSTR(x)

// MQTT Configuration (DON'T LEAK THIS TO GITHUB!!!)
const char* MQTT_HOSTNAME = "mqtt.gruppe1.tech";
const char* MQTT_TOPIC = "devices/1/location";
const int MQTT_PORT1 = 9002;
const char* MQTT_PASSWORD = STR(MQTT_PASSWD);
const char* MQTT_USER = STR(MQTT_USERNAME);

// Wi-Fi Configuration (DON'T LEAK THIS TO GITHUB!!!)
const char* ssid = "NTNU-IOT";  // Your Wi-Fi SSID
const char* pass = "";  // Your Wi-Fi password

WebSocketsClient client;
MQTTPubSubClient mqtt;

// Define the pins for PWM control
const int freq = 10000;            // PWM frequency in Hz
const int ledChannel0 = 0;       // PWM channel for motor 1
const int ledChannel1 = 1;       // PWM channel for motor 2
const int ledChannel2 = 2;       // PWM channel for motor 3
const int resolution = 12;       // PWM resolution (12 bits)

// Define the pins for the motors
const int motorPin1 = 3;        // GPIO pin for motor 1
const int motorPin2 = 17;        // GPIO pin for motor 2
const int motorPin3 = 18;        // GPIO pin for motor 3

// Set up the pins for the RFID reader
#define RST_PIN         1        // Reset pin for MFRC522
#define SS_PIN          9        // Slave select pin for MFRC522
#define MOSI_PIN        2        // MOSI pin for SPI
#define MISO_PIN        11       // MISO pin for SPI
#define SCK_PIN         10       // SCK pin for SPI
#define BUZZER_PIN      5        // Buzzer pin

// Set up the pins for the distance sensors
#define SDA_PIN         13       // SDA pin for I2C
#define SCL_PIN         12       // SCL pin for I2C
#define XSHUT_PIN_1     4        // XSHUT pin for sensor 1
#define XSHUT_PIN_2     6        // XSHUT pin for sensor 2
#define XSHUT_PIN_3     7        // XSHUT pin for sensor 3

// Create VL53L0X sensor instances
VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Define distance range for mapping
const uint16_t minDistance = 50;     // Minimum distance in mm (closest)
const uint16_t maxDistance = 500;    // Maximum distance in mm (farthest)

// Function to map distance to duty cycle
uint32_t mapDistanceToDutyCycle(uint16_t distance) {
    // Clamp the distance to the specified range
    if (distance > maxDistance) {
        distance = maxDistance;
    }
    if (distance < minDistance) {
        distance = minDistance;
    }

    // Calculate the normalized value (0.0 to 1.0)
    float normalized = (float)(maxDistance - distance) / (float)(maxDistance - minDistance);

    // Calculate the duty cycle based on resolution
    uint32_t maxDutyCycle = (1 << resolution) - 1;  // e.g., 4095 for 12-bit resolution

    // Calculate the duty cycle
    uint32_t dutyCycle = (uint32_t)(normalized * maxDutyCycle);

    // Ensure duty cycle is within valid range
    if (dutyCycle > maxDutyCycle) {
        dutyCycle = maxDutyCycle;
    }
    if (dutyCycle < 0) {
        dutyCycle = 0;
    }

    return dutyCycle;
}

// Function to handle Wi-Fi and MQTT connections
void connect() {
connect_to_wifi:
    Serial.print("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(" connected!");

connect_to_host:
    Serial.println("Connecting to MQTT broker...");
    client.disconnect();
    client.begin(MQTT_HOSTNAME, MQTT_PORT1, "/", "mqtt");  // "mqtt" is required
    client.setReconnectInterval(2000);

    Serial.print("Connecting to MQTT broker...");
    while (!mqtt.connect("esp32-client", MQTT_USER, MQTT_PASSWORD)) {
        Serial.print(".");
        delay(1000);
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Wi-Fi disconnected");
            goto connect_to_wifi;
        }
        if (!client.isConnected()) {
            Serial.println("WebSocket client disconnected");
            goto connect_to_host;
        }
    }
    Serial.println(" connected!");
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Initialize I2C bus
    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialize XSHUT pins
    pinMode(XSHUT_PIN_1, OUTPUT);
    pinMode(XSHUT_PIN_2, OUTPUT);
    pinMode(XSHUT_PIN_3, OUTPUT);

    // Shutdown all sensors
    digitalWrite(XSHUT_PIN_1, LOW);
    digitalWrite(XSHUT_PIN_2, LOW);
    digitalWrite(XSHUT_PIN_3, LOW);
    delay(10);  // Ensure sensors are in shutdown

    // Initialize and assign new address to sensor 1
    digitalWrite(XSHUT_PIN_1, HIGH);
    delay(10);  // Wait for sensor to boot up
    sensor1.setAddress(0x30);  // New I2C address for sensor 1
    sensor1.init();
    sensor1.startContinuous();

    // Initialize and assign new address to sensor 2
    digitalWrite(XSHUT_PIN_2, HIGH);
    delay(10);  // Wait for sensor to boot up
    sensor2.setAddress(0x31);  // New I2C address for sensor 2
    sensor2.init();
    sensor2.startContinuous();

    // Initialize and assign new address to sensor 3
    digitalWrite(XSHUT_PIN_3, HIGH);
    delay(10);  // Wait for sensor to boot up
    sensor3.setAddress(0x32);  // New I2C address for sensor 3
    sensor3.init();
    sensor3.startContinuous();

    // PWM setup for motors
    ledcSetup(ledChannel0, freq, resolution);
    ledcSetup(ledChannel1, freq, resolution);
    ledcSetup(ledChannel2, freq, resolution);

    // Attach the motors to the PWM channels
    ledcAttachPin(motorPin1, ledChannel0);
    ledcAttachPin(motorPin2, ledChannel1);
    ledcAttachPin(motorPin3, ledChannel2);

    // Ensure the motors are off
    ledcWrite(ledChannel0, 0);
    ledcWrite(ledChannel1, 0);
    ledcWrite(ledChannel2, 0);

    // Play startup sound
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

    // Initialize SPI bus for RFID reader
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

    // Initialize MFRC522 RFID reader
    mfrc522.PCD_Init();

    // Initialize MQTT client
    mqtt.begin(client);

    // Connect to Wi-Fi, host, and MQTT broker
    connect();
}

// Timing variables
unsigned long TOFMillis = 0;
const unsigned long TOFInterval = 100;           // Interval for sensor readings
const uint32_t cardReadInterval = 1000;          // Interval for RFID reading

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

            // Publish the UID via MQTT
            mqtt.publish(MQTT_TOPIC, uidString);

            Serial.println(F("\n**End Reading**\n"));

            // Halt PICC and stop encryption on PCD
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
        }
        // No else needed; if no card is present, simply continue
    }

    // Sensor reading and motor control code
    if (currentMillis - TOFMillis >= TOFInterval) {
        TOFMillis = currentMillis;

        // Read from sensor 1
        uint16_t distance1 = sensor1.readRangeContinuousMillimeters();
        if (sensor1.timeoutOccurred()) {
            Serial.println("Sensor 1 timeout!");
            // Set motor to off
            ledcWrite(ledChannel0, 0);
        } else {
            Serial.print("Distance 1: ");
            Serial.print(distance1);
            Serial.println(" mm");
            // Map distance to duty cycle
            uint32_t dutyCycle1 = mapDistanceToDutyCycle(distance1);
            // Set motor PWM
            ledcWrite(ledChannel0, dutyCycle1);
        }

        // Read from sensor 2
        uint16_t distance2 = sensor2.readRangeContinuousMillimeters();
        if (sensor2.timeoutOccurred()) {
            Serial.println("Sensor 2 timeout!");
            // Set motor to off
            ledcWrite(ledChannel1, 0);
        } else {
            Serial.print("Distance 2: ");
            Serial.print(distance2);
            Serial.println(" mm");
            // Map distance to duty cycle
            uint32_t dutyCycle2 = mapDistanceToDutyCycle(distance2);
            // Set motor PWM
            ledcWrite(ledChannel1, dutyCycle2);
        }

        // Read from sensor 3
        uint16_t distance3 = sensor3.readRangeContinuousMillimeters();
        if (sensor3.timeoutOccurred()) {
            Serial.println("Sensor 3 timeout!");
            // Set motor to off
            ledcWrite(ledChannel2, 0);
        } else {
            Serial.print("Distance 3: ");
            Serial.print(distance3);
            Serial.println(" mm");
            // Map distance to duty cycle
            uint32_t dutyCycle3 = mapDistanceToDutyCycle(distance3);
            // Set motor PWM
            ledcWrite(ledChannel2, dutyCycle3);
        }
    }
}
