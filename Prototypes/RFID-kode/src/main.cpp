#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Define your SPI and MFRC522 pins
//Disse må nok endres før koden passer perfekt
#define RST_PIN         13    // RST pin connected to GPIO39
#define SS_PIN          9    // SS (SDA) pin connected to GPIO14
#define MOSI_PIN        11    // MOSI pin connected to GPIO16
#define MISO_PIN        12     // MISO pin connected to GPIO2
#define SCK_PIN         10     // SCK pin connected to GPIO1

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
    Serial.begin(115200);    // Initialize serial communications with the PC
    while (!Serial);         // Wait for the serial port to open
    // Initialize SPI bus with specified pins
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    // Initialize MFRC522
    mfrc522.PCD_Init();
    delay(5);                // Optional delay
    // Show details of PCD - MFRC522 Card Reader details
    mfrc522.PCD_DumpVersionToSerial();
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
    // Reset the loop if no new card present on the sensor/reader.
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    // Only dumps UID
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}
