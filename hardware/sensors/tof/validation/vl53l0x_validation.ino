#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup() {
  Serial.begin(115200);

  Wire.begin(13, 15);

  if (!sensor.init()) {
    Serial.println("VL53L0X NOT FOUND");
    while (1);
  }

  sensor.startContinuous();

  Serial.println("VL53L0X FOUND");
}

void loop() {
  Serial.print("Distance: ");
  Serial.print(sensor.readRangeContinuousMillimeters());
  Serial.println(" mm");

  delay(500);
}