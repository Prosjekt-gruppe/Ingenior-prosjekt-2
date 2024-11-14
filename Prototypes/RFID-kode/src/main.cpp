#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Define your SPI and MFRC522 pins
//Disse må nok endres før koden passer perfekt
#define RST_PIN         4    // RST pin connected to GPIO4
#define SS_PIN          5    // SS (SDA) pin connected to GPIO5
#define MOSI_PIN        23   // MOSI pin connected to GPIO23
#define MISO_PIN        19   // MISO pin connected to GPIO19
#define SCK_PIN         18   // SCK pin connected to GPIO18

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
