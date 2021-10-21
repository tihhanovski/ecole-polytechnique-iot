#include <Wire.h>

/* 
  We will agree on enumerating the different states that the traffic light can have, as follows:
    1. Red
    2. Red-Yellow
    3. Green
    4. Yellow

  So internally we use patterns numbers 0 .. 3,
  But we will transmit numbers 1 to 4 to meet requirements.

  Network protocol is very simple:
  Master send to slave one byte with current pattern number (1 .. 4)
  When master requests the data from slave, slave will return one byte with current pattern number (1 .. 4)
*/

// Here we save current pattern of first slave between iterations
// Second light's pattern is two steps ahead (eg Red -> Green)
uint8_t currentPattern;

// Slaves' addresses
const uint8_t slaves[] = {10, 11};

void setup() 
{
  Wire.begin();
  Serial.begin(9600);

  currentPattern = 0;
}

void loop() {

  // Retrieve patterns from slaves
  int patterns[2] = {0, 0};           // Here we will save patterns retrieved to compare them later

  uint8_t i = 0;
  for(int slaveId : slaves)
  {
    Serial.print("Request pattern from slave #");
    Serial.print(slaveId);
    Serial.print(": ");
    Wire.requestFrom(slaveId, 1);     // see example https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterReader
    while (Wire.available()) {        // slave may send less than requested
      patterns[i] = Wire.read();      // receive a byte as int
      Serial.print(patterns[i]);      // print the character
    }
    Serial.println(";");
    i++;
  }

  // Sample code to compare retrieved patterns (we have only two slaves, so the code is simple)
  // Here we just output message to Serial port
  // Also we can move this code to check slaves state after.
  // Also checks could be more complex, for example we should avoid yellow on one light and green on the other?
  if(patterns[0] == patterns[1])
  {
    Serial.print("***** Something is wrong!!! Both traffic lights have same pattern: ");
    Serial.println(patterns[0]);
  }

  // Send patterns to slaves
  // We have two traffic lights, so we just shift second pattern two steps further.
  uint8_t p = currentPattern;               // Get current pattern
  for(uint8_t slaveId : slaves)             // Scroll the slaves list (works with only two slaves)
  {
    Serial.print("Send pattern ");
    Serial.print(p + 1);
    Serial.print(" to slave with id=");
    Serial.println(slaveId);

    Wire.beginTransmission(slaveId);      // Connect to slave
    Wire.write(p + 1);                    // Send the pattern.
    Wire.endTransmission();               // Disconnect
    p = (p + 2) % 4;                      // Shift pattern for next slave
  }

  currentPattern = (currentPattern + 1) % 4;    // Step the current pattern 
  delay(2000);                                  // Wait for 2 seconds
  Serial.println("-------");

}