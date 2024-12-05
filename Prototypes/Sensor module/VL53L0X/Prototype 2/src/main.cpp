#include "Arduino.h"
#include "Wire.h"
#include <VL53L0X.h>

// Define the I2C pins
#define SDA_PIN 21
#define SCL_PIN 22

// Define XSHUT pin for VL53L0X 
#define XSHUT_PIN_1 16 
#define XSHUT_PIN_2 17

// Initialize the VL53L0X sensor
VL53L0X LoX1;
VL53L0X LoX2;


// Define sensor address
#define SENSOR1_ADDRESS 0x30
#define SENSOR2_ADDRESS 0x31

// Define the time interval
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);

  // Configure XSHUT pins
  pinMode(XSHUT_PIN_1, OUTPUT);
  pinMode(XSHUT_PIN_2, OUTPUT);

  // Reset VL53L0X sensors
  digitalWrite(XSHUT_PIN_1, LOW);
  digitalWrite(XSHUT_PIN_2, LOW);

  // Initialize the VL53L0X sensor 1
  digitalWrite(XSHUT_PIN_1, HIGH);
  delay(10);
  LoX1.setAddress(SENSOR1_ADDRESS);
  if (!LoX1.init()) {
    Serial.println("Failed to initialize VL53L0X sensor 1!");
    while (1);
  }

  // Initialize the VL53L0X sensor 1
  digitalWrite(XSHUT_PIN_2, HIGH);
  delay(10);
  LoX2.setAddress(SENSOR2_ADDRESS);
  if (!LoX2.init()) {
    Serial.println("Failed to initialize VL53L0X sensor 2!");
    while (1);
  }
  // Start continuous ranging
  LoX1.startContinuous();
  LoX2.startContinuous();
  
}


void loop(){

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read the distance
    uint16_t distance1 = LoX1.readRangeContinuousMillimeters();
    uint16_t distance2 = LoX2.readRangeContinuousMillimeters();
    
    
    // Print the distance
    Serial.print("Distance 1: ");
    Serial.print(distance1);
    Serial.print(" mm");

    Serial.print("      ");

    Serial.print("Distance 2: ");
    Serial.print(distance2);
    Serial.println(" mm");
  }
}