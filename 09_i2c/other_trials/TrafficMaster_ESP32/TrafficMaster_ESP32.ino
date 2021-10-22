#include "Wire.h"

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  byte error, address;
  int nDevices = 0;

  delay(5000);

  Serial.println("Scanning for I2C devices ...");
  for(address = 0x01; address < 0x7f; address++){
    Serial.print(" ");
    Serial.print(address);
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Serial.printf("\nI2C device found at address 0x%02X\n", address);
      nDevices++;
    } else if(error != 2){
      Serial.printf("\nError %d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0){
    Serial.println("\nNo I2C devices found");
  }
}