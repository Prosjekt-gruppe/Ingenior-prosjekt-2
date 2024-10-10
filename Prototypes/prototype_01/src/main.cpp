#include <Arduino.h>
// Ultra-sonic sensor
#define TRIG_ECHO_PIN 27
// PWM
const int ledChannel = 0;
const int ledPin = 18;
const int frequency = 5000;
const int resolution = 8;
long microsecondsToCentimeters(long microseconds);
long getDistance();
long normalizeDistance();
void setup() {
  Serial.begin(115200);
  ledcSetup(ledChannel, frequency, resolution);
  ledcAttachPin(ledPin, ledChannel);
}
void loop() {
  long strength = normalizeDistance();
  analogWrite(ledPin, strength);
  Serial.println(strength);
  delay(500);
}
long normalizeDistance() {
  const int maxDistance = 60;
  long x = getDistance();
  if (x > maxDistance) {
    return 0;
  }
  
  return map(x, 0, maxDistance, 255, 0);
}
long getDistance() {
  long duration, inches, cm;
  pinMode(TRIG_ECHO_PIN, OUTPUT);
  digitalWrite(TRIG_ECHO_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_ECHO_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG_ECHO_PIN, LOW);
  pinMode(TRIG_ECHO_PIN, INPUT);
  duration = pulseIn(TRIG_ECHO_PIN, HIGH);
  return microsecondsToCentimeters(duration); 
}
long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
