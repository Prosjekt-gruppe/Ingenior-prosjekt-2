#include <Adafruit_VL53L0X.h>
#include <Wire.h>

// Opprett et objekt for Adafruit VL53L0X
Adafruit_VL53L0X sensor = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  // Start I2C på spesifikke pinner for ESP32-S3 (tilpass om nødvendig)
  Wire.begin(9, 10); // Bruk GPIO8 som SDA og GPIO7 som SCL, eller bruk de pinnene som fungerer for deg

  // Initialiser sensoren og sjekk tilkoblingen
  if (!sensor.begin()) {
    Serial.println("Klarte ikke å starte VL53L0X!");
    while (1); // Stopper hvis sensoren ikke kan initialiseres
  }

  Serial.println("Sensor initialisert og klar.");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  // Utfør en enkel måling
  sensor.rangingTest(&measure, false);

  // Sjekk om målingen er gyldig
  if (measure.RangeStatus != 4) { // 4 indikerer ugyldig måling
    Serial.print("Avstand (mm): ");
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println("Utenfor rekkevidde");
  }

  delay(500); // Vent litt før neste måling
}
