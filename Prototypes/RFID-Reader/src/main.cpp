#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         4    // RST pin connected to GPIO4
#define SS_PIN          5    // SS (SDA) pin connected to GPIO5
#define MOSI_PIN        23   // MOSI pin connected to GPIO23
#define MISO_PIN        19   // MISO pin connected to GPIO19
#define SCK_PIN         18   // SCK pin connected to GPIO18

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//*****************************************************************************************//
void setup() {
  Serial.begin(115200);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card
  Serial.println(F("Read UID: "));    //shows in serial that it is ready to read
}

//*****************************************************************************************//
void loop() {

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  //-------------------------------------------

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  Serial.println(F("\n**End Reading**\n"));

  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
//*****************************************************************************************//
