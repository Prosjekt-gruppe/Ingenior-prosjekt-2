//This is the current code to be used, the rest are just there for info about the card.

//Include the necessary libraries
#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <WebSocketsClient.h>  // include before MQTTPubSubClient.h
#include <MQTTPubSubClient.h>
#include <MFRC522.h>

//Set up a converter so that the Authentification key can be read
#define XSTR(x) #x
#define STR(x) XSTR(x)

//Set up the MQTT hostname and topic
const char* MQTT_HOSTNAME = "mqtt.gruppe1.tech";
const char* MQTT_TOPIC = "test/hello";

//Set up the wifi
const char* ssid = "NTNU-IOT";
const char* pass = "";

//Set up the WebSocketsClient and the MQTTPubSubClient
WebSocketsClient client;
MQTTPubSubClient mqtt;

//Set up the pins
#define RST_PIN         39    // RST pin connected to GPIO4
#define SS_PIN          14    // SS (SDA) pin connected to GPIO5
#define MOSI_PIN        16   // MOSI pin connected to GPIO23
#define MISO_PIN        2   // MISO pin connected to GPIO19
#define SCK_PIN         1   // SCK pin connected to GPIO18

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);


//Function to connect to the wifi and the host
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
    while (!mqtt.connect("esp32-client", "midjo", "MidjosLambs")) {
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

  //Connect to the wifi and the mqtt broker
  WiFi.begin(ssid, pass);
  mqtt.begin(client);
  connect();

  //Set up the reader
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("Read UID: "));
}


void loop() {
  mqtt.update();

//WORK IN PROGRESS HERE!!!

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

  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));      //uncomment this to see all blocks in hex

  //-------------------------------------------

  Serial.println(F("\n**End Reading**\n"));

  delay(1000); //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

