#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  Wire.requestFrom(10, 5);
  while (Wire.available()) {
    Serial.print((char)Wire.read());
  }
  Serial.println();
  delay(1000);
}