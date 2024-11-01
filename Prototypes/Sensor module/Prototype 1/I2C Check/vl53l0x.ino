#include <Wire.h>

void setup() {
  Serial.begin(115200);
  
  // Start I2C på de spesifikke I2C-pinnene for ESP32-S3 (tilpass om nødvendig)
  Wire.begin(9, 10); // Bruker GPIO8 som SDA og GPIO7 som SCL

  Serial.println("I2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning...");

  // Sjekker alle mulige adresser fra 1 til 127
  for (address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      Serial.println(address, HEX);
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }

  delay(2000); // Vent litt før neste scanning
}
