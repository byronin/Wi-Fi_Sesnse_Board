#include "Adafruit_VL53L0X.h" //- https://github.com/adafruit/Adafruit_VL53L0X

Adafruit_VL53L0X lox ;
VL53L0X_RangingMeasurementData_t measure;
void setup() {

  pinMode(15, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(16, OUTPUT);

  Serial.begin(115200);
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1);
  }

}


void loop() {
  lox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4 && measure.RangeMilliMeter != 8191) {
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println("-1");
  }
  if (measure.RangeMilliMeter < 700) {
    digitalWrite(15, HIGH);
  } else {
    digitalWrite(15, LOW);
  }
  if (measure.RangeMilliMeter < 450) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  if (measure.RangeMilliMeter < 300) {
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);
  }
  if (measure.RangeMilliMeter < 125) {
    digitalWrite(16, HIGH);
  } else {
    digitalWrite(16, LOW);
  }
  if (measure.RangeMilliMeter == -1) {
    digitalWrite(16, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    digitalWrite(15, LOW);
  } 
  delay(100);
}
