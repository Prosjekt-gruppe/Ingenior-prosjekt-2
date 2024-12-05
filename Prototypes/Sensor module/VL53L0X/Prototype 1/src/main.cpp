#include "Arduino.h"
#include "Wire.h"
#include <VL53L0X.h>

// Define the I2C pins
#define SDA_PIN 21
#define SCL_PIN 22

// Initialize the VL53L0X sensor
VL53L0X sensor;

// Define the time interval
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);

  // Initialize the VL53L0X sensor
  if (!sensor.init()) {
    Serial.println("Failed to initialize VL53L0X sensor!");
    while (1);
  }

  // Start continuous ranging
  sensor.startContinuous();
}


void loop(){

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read the distance
    uint16_t distance = sensor.readRangeContinuousMillimeters();
    
    // Print the distance
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" mm");
  }
}