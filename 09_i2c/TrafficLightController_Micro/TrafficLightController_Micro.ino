#include <Wire.h>

/* We will agree on enumerating the different states that the traffic light can have, as follows:
    1. Red
    2. Red-Yellow
    3. Green
    4. Yellow
*/

uint8_t currentPattern;

const uint8_t slaves[] = {10, 11};

void setup() 
{
  Wire.begin();
  Serial.begin(9600);

  currentPattern = 0;
}

void loop() {

  //retrieve patterns from slaves
  int patterns[2] = {0, 0};

  uint8_t i = 0;
  for(int slaveId : slaves)
  {
    Serial.print("Request pattern from slave #");
    Serial.print(slaveId);
    Serial.print(": ");
    Wire.requestFrom(slaveId, 1); // see example https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterReader
    while (Wire.available()) {    // slave may send less than requested
      patterns[i] = Wire.read();      // receive a byte as int
      Serial.print(patterns[i]);         // print the character
    }
    Serial.println(";");
    i++;
  }

  if(patterns[0] == patterns[1])
  {
    Serial.print("***** Something is wrong!!! Both traffic lights have same pattern: ");
    Serial.println(patterns[0]);
  }


  uint8_t p = currentPattern;
  uint8_t delta = 0;
  for(uint8_t slaveId : slaves)
  {
    Serial.print("Send pattern ");
    Serial.print(p + 1);
    Serial.print(" to slave with id=");
    Serial.println(slaveId);

    Wire.beginTransmission(slaveId);
    Wire.write(p + 1);
    Wire.endTransmission();
    delta++;
    p = (p + 2) % 4;
  }

  currentPattern = (currentPattern + 1) % 4;
  delay(2000);
  Serial.println("-------");

  /*
  Wire.requestFrom(10, 5);
  while (Wire.available()) {
    Serial.print((char)Wire.read());
  }
  Serial.println();
  delay(1000);
  */
}