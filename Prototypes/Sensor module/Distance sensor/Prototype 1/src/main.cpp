#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

// Define I2C pins
#define SDA_PIN 21
#define SCL_PIN 22

// Define XSHUT pin
#define XSHUT_PIN 16



// Create VL53L0X object
VL53L0X LOX1;


// Sensor I2C address
#define SENSOR_ADDRESS 0x30

// Time between sensor readings
unsigned long previousMillis = 0;
const unsigned long interval = 100;


// Function to scan for I2C devices
void I2Cscan() {
  byte error, address;
  int nDevices = 0;
 
  Serial.println("Scanning...");
 
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
   
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
  }
 
  if (nDevices > 0) {
    Serial.println("done");
  } else {
    Serial.println("No I2C devices found");
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Configure XSHUT pin as an output
  pinMode(XSHUT_PIN, OUTPUT);

  // Reset sensor
  digitalWrite(XSHUT_PIN, LOW);
  delay(10);

  // Initialize sensor
  digitalWrite(XSHUT_PIN, HIGH);
  delay(10);

  Serial.print("Default address for sensor: 0x");
  Serial.println(LOX.getAddress(), HEX);
  if (!LOX.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while(1);
  }

  LOX.setAddress(SENSOR_ADDRESS);
  Serial.print("Changed address to: 0x");
  Serial.println(LOX.getAddress(), HEX);
  Serial.println();

  Serial.println("Scanning for I2C devices...");
  I2Cscan();
}

void loop() {

}

