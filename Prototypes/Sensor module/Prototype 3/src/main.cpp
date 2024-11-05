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

const int motorPin = 1; // GPIO for Vibration motor 
const int pwmChannel = 0;   // PWM channel
const int pwmFrequency = 1000;  // PWM frequency
const int pwmResolution = 8;    // PWM resolution (8 bits = 0-255 duty cycle)

// Setup for PWM
void pwmSetup(){
    ledcAttachPin(motorPin, pwmChannel);
    ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
}

// Function to set vibration intensity based on distance
void setIntensity(int distance){
    int intensity = map(distance, 300, 0, 0, 255);
    intensity = constrain(intensity,0,255);

    ledcWrite(pwmChannel, intensity);

    Serial.print("Distance (mm): ");
    Serial.print(distance);
    Serial.print("  Vibration Intensity: ");
    Serial.println(intensity); 
}



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

    // Trigger vibration motor if distance is below 300 mm
    if (distance < 300) {
      setIntensity(distance);
    } else {
      ledcWrite(pwmChannel,0); // Turn off the vibration motor
    }
  } else {
    Serial.println("Out of reach");
    ledcWrite(pwmChannel,0); // Ensure motor is off if measurement is out of range
  }
}



/*--------------------*/
/*--------TEST--------*/
/*--------------------*/

//////////Empty/////////



void setup() {
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT); // Pinout for Vibration motor

  // Start I2C on specific GPIO pins
  Wire.begin(loxSDA, loxSCL);

  // Setup for PWM 
  pwmSetup();

  // Setup for distance sensor
  loxSetup();
}

void loop() {
    alarm();
    delay(50); // Wait to feel the effect  
}