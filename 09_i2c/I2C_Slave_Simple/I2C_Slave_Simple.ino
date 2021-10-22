#include <Wire.h>
#define DIGITALIN 11

void setup() {
  Wire.begin(10);
  //Wire.onRequest(requestEvent);
}
void loop() {
  delay(200);
}
void requestEvent() {
  if (digitalRead(DIGITALIN) == HIGH){
    Wire.write("HIGH");
  } else{
    Wire.write("low ");
  }
}