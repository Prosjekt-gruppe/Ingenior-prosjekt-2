//These RFID tags cannot be written to, so this code will be for testing wirless communication.

#include <Arduino.h>
#include <Wire.h>
#include "mqtt_client.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

const char* WIFI_PASSWD = STR(WIFI_PASSWD);
const char* WIFI_SSID = STR(WIFI_SSID);

const char* MQTT_HOSTNAME = "mqtt.gruppe1.tech";

//const char* MQTT_HOST = STR(MQTT_HOST);
const uint32_t PORT_WITHOUT_ENC = (uint32_t)STR(MQTT_PORT);

const char* MQTT_USERNAME = STR(MQTT_USERNAME);
const char* MQTT_PASSWD = STR(MQTT_PASSWD);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World");
  delay(1000);
}