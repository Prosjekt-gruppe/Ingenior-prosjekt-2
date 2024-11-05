#include <Adafruit_VL53L0X.h>
#include <Wire.h>


/*-------------------------------*/
/*--------Distance Sensor--------*/
/*-------------------------------*/

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
const int loxSDA = 8;   //GPIO for SDA
const int loxSCL = 9;   //GPIO for SCL


// Initialization of distance sensor
void loxSetup(){
    if (!lox.begin()) {
        Serial.println("Klarte ikke å starte VL53L0X!");
        while (1); // Stopper hvis sensoren ikke kan initialiseres
        }
        Serial.println("Sensor initialisert og klar.");
}



/*-------------------------------*/
/*--------Vibration Motor--------*/
/*-------------------------------*/

const int vibPin = 1; // GPIO for Vibration motor 


/*----------------------------*/
/*-----------ALARM------------*/
/*----------------------------*/

// Function to activate alarm if distance is below threshold
void alarm() {
  VL53L0X_RangingMeasurementData_t measure;

  // Measurement function
  lox.rangingTest(&measure, false);

  // Check if the measurement is valid
  if (measure.RangeStatus != 4) { // 4 indicates an invalid measurement
    int distance = measure.RangeMilliMeter;

    Serial.print("Distance (mm): ");
    Serial.println(distance);

    // Trigger vibration motor if distance is below 300 mm
    if (distance < 300) {
      digitalWrite(vibPin, HIGH); // Turn on the vibration motor
    } else {
      digitalWrite(vibPin, LOW); // Turn off the vibration motor
    }
  } else {
    Serial.println("Out of reach");
    digitalWrite(vibPin, LOW); // Ensure motor is off if measurement is out of range
  }
}



/*--------------------*/
/*--------TEST--------*/
/*--------------------*/

//////////Empty/////////



void setup() {
  Serial.begin(115200);
  pinMode(vibPin, OUTPUT); // Pinout for Vibration motor

  // Start I2C on specific GPIO pins
  Wire.begin(loxSDA, loxSCL);
  // Setup for distance sensor
  loxSetup();
}

void loop() {
    alarm();
    delay(50); // Wait to feel the effect  
}