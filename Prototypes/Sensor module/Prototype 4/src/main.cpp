#include <Adafruit_VL53L0X.h>
#include <Wire.h>


/*----------------------------*/
/*------Distance Sensors------*/
/*----------------------------*/

// Distance Sensor setup
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X(); // Working
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X(); // Not Checked
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X(); // Not Checked

// Distance Sensor 1 (SDA: 8, SCL: 9)
const int loxSDA1 = 8;   
const int loxSCL1 = 9;   

// Distance Sensor 2 (SDA: 10, SCL: 11)
const int loxSDA2 = 10;   
const int loxSCL2 = 11;   

// Distance Sensor 3 (SDA: 12, SCL: 13)
const int loxSDA3 = 12;   
const int loxSCL3 = 13;   


// Initialization of distance sensor
void loxSetup() {

  // Initialize sensor 1
  Wire.begin(loxSDA1, loxSCL1);
  if (!lox1.begin()) {
    Serial.println("Could not initialize VL53L0X sensor 1");
    while (1);
  }
  Serial.println("Sensor 1 initialized");


  // Initialize sensor 2
  Wire.begin(loxSDA2, loxSCL2);
  if (!lox2.begin()) {
    Serial.println("Could not initialize VL53L0X sensor 2");
    while (1);
  }
  Serial.println("Sensor 2 initialized");


  // Initialize sensor 3
  Wire.begin(loxSDA3, loxSCL3);
  if (!lox3.begin()) {
    Serial.println("Could not initialize VL53L0X sensor 3");
    while (1);
  }
  Serial.println("Sensor 3 initialized");
}




// Vibration Motor setup
const int vibPin0 = 3;  // GPIO for Vibration motor 1 (PWM) Testing 
const int vibPin1 = 4;  // GPIO for Vibration motor 2 (PWM)
const int vibPin2 = 5;  // GPIO for Vibration motor 3 (PWM)
const int vibPin3 = 6;  // GPIO for Vibration motor 4 (PWM)
const int vibPin4 = 7;  // GPIO for Vibration motor 5 (PWM)


// Function to activate vibration motor if distance is below 300mm
// Parameters:
// - distanceSensor: the sensor object to take measurements from
// - vibPin: the vibration pin to control the motor
// - intensityCoeff: the intensity multiplier (1-10) to adjust vibration feedback

void controlVibration(Adafruit_VL53L0X& distanceSensor, int vibPin, int intensityCoeff, bool debugMode) {
  VL53L0X_RangingMeasurementData_t measure;
  
  // Measure the distance
  distanceSensor.rangingTest(&measure, false);

  // Check if the measurement is valid
  if (measure.RangeStatus != 4) { // 4 indicates an invalid measurement
    int distance = measure.RangeMilliMeter;
    // Print distance values for debugging if debugMode is true
    if (debugMode) {
      Serial.print("Distance (mm): ");
      Serial.println(distance);
    }

    // Trigger vibration motor if distance is below 300 mm
    if (distance < 300) {
      // Adjust intensity using a multiplier (intensityCoeff)
      // The map range is modified to give a range between 0 and 255 (PWM)
      int intensity = map(distance, 0, 300, 255, 0); // Map distance to PWM value
      intensity = intensity * intensityCoeff; // Apply intensity coefficient

      // Ensure intensity stays within valid PWM range (0 to 255)
      if (intensity > 255) intensity = 255;
      if (intensity < 0) intensity = 0;


      Serial.println("Inensity:");
      Serial.println(intensity);

      analogWrite(vibPin, intensity);  // Set the motor vibration intensity for the selected motor pin
    } else {
      analogWrite(vibPin, 0); // Turn off the motor if distance is above 300mm
    }
  } else {
    if (debugMode) {
      Serial.println("Out of reach");
    }
    analogWrite(vibPin, 0); // Ensure motor is off if measurement is out of range
  }
}

void setup() {
  Serial.begin(115200);
  // Setup distance sensors
  loxSetup();

}

void loop() {
  // Call the controlVibration function with appropriate parameters:

  // First sensor controls vibPin0 and vibPin1 
  controlVibration(lox1, vibPin0, 1, false);  // Debug mode enabled

  /*
  // Second sensor controls vibPin0 and vibPin1 
  controlVibration(lox2, vibPin3, 5, false);  // Debug mode enabled

  // Third sensor controls vibPin0 and vibPin1 
  controlVibration(lox3, vibPin4, 5, true);  // Debug mode enabled

  */


  delay(50);  // Delay to avoid flooding the serial monitor
}
