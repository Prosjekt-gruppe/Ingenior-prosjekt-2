//Include the necessary libraries
#include <Arduino.h>
#include <Wire.h>

#define SDA_PIN 13  // Replace with your chosen SDA pin
#define SCL_PIN 12  // Replace with your chosen SCL pin

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN); 
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}
 
void loop() {
  byte error, address; //variable for error and I2C address
  int nDevices; //variable for number of devices
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}
