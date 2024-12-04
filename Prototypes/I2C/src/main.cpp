#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

#define SDA_PIN 13
#define SCL_PIN 12

#define XSHUT_PIN_1 4
#define XSHUT_PIN_2 6
#define XSHUT_PIN_3 7

VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;

unsigned long previousMillis = 0;
const unsigned long interval = 100;  // Time between sensor readings (in milliseconds)

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C bus
  Serial.begin(115200);

  // Initialize XSHUT pins
  pinMode(XSHUT_PIN_1, OUTPUT);
  pinMode(XSHUT_PIN_2, OUTPUT);
  pinMode(XSHUT_PIN_3, OUTPUT);

  // Shutdown all sensors
  digitalWrite(XSHUT_PIN_1, LOW);
  digitalWrite(XSHUT_PIN_2, LOW);
  digitalWrite(XSHUT_PIN_3, LOW);
  delay(10);  // Short delay to ensure sensors are in shutdown

  // Initialize and assign new address to sensor 1
  digitalWrite(XSHUT_PIN_1, HIGH);
  delay(10);  // Wait for the sensor to boot up
  sensor1.setAddress(0x30);  // New I2C address for sensor 1
  sensor1.init();
  sensor1.startContinuous();

  // Initialize and assign new address to sensor 2
  digitalWrite(XSHUT_PIN_2, HIGH);
  delay(10);  // Wait for the sensor to boot up
  sensor2.setAddress(0x31);  // New I2C address for sensor 2
  sensor2.init();
  sensor2.startContinuous();

  // Initialize and assign new address to sensor 3
  digitalWrite(XSHUT_PIN_3, HIGH);
  delay(10);  // Wait for the sensor to boot up
  sensor3.setAddress(0x32);  // New I2C address for sensor 3
  sensor3.init();
  sensor3.startContinuous();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read from sensor 1
    uint16_t distance1 = sensor1.readRangeContinuousMillimeters();
    if (sensor1.timeoutOccurred()) {
      Serial.println("Sensor 1 timeout!");
    } else {
      Serial.print("Distance 1: ");
      Serial.print(distance1);
      Serial.println(" mm");
    }

    // Read from sensor 2
    uint16_t distance2 = sensor2.readRangeContinuousMillimeters();
    if (sensor2.timeoutOccurred()) {
      Serial.println("Sensor 2 timeout!");
    } else {
      Serial.print("Distance 2: ");
      Serial.print(distance2);
      Serial.println(" mm");
    }

    // Read from sensor 3
    uint16_t distance3 = sensor3.readRangeContinuousMillimeters();
    if (sensor3.timeoutOccurred()) {
      Serial.println("Sensor 3 timeout!");
    } else {
      Serial.print("Distance 3: ");
      Serial.print(distance3);
      Serial.println(" mm");
    }
  }
}
