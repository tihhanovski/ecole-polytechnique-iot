#include <Wire.h>

#define SWITCH_DELAY 2000
#define SLAVE_POLL_DELAY 100

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
int currentPattern;
unsigned long patternChangeTimer;
unsigned long slavePollTimer;

// Slaves' addresses
const uint8_t slaves[] = {10, 11};
bool greenRequested[2] = {false, false};

void setup() 
{
  Wire.begin();
  Serial.begin(9600);

  currentPattern = -1;
  patternChangeTimer = 0;
  slavePollTimer = 0;
}

int patterns[2] = {0, 0};           // Here we will save patterns retrieved to compare them later

void pollSlaves()
{
  // Retrieve patterns from slaves
  if(millis() > slavePollTimer)
  {
    uint8_t i = 0;
    for(int slaveId : slaves)
    {
      patterns[i] = 0;      
      #ifdef DEBUG
        Serial.print("Request pattern from slave #");
        Serial.print(slaveId);
        Serial.print(": ");
      #endif
      Wire.requestFrom(slaveId, 1);           // see example https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterReader
      while (Wire.available()) {              // slave may send less than requested
        uint8_t packet = Wire.read();         // receive a byte as uint8_t 
        #ifdef DEBUG
          Serial.print("-->[");
          Serial.print(packet, BIN);
          Serial.print("]-->");
        #endif

        if((!greenRequested[i])&&(0b1000 & packet))
        {
          greenRequested[i] = true;  //if third bit is set then green is requested
          Serial.print("Slave #");
          Serial.print(slaves[i]);
          Serial.println(" requested green");
        }
        patterns[i] = 0b0111 & packet;        // receive a byte as int
        #ifdef DEBUG
          Serial.print(patterns[i]);            // print the character
        #endif
      }
      #ifdef DEBUG
        Serial.println(";");
      #endif
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

    slavePollTimer = millis() + SLAVE_POLL_DELAY;
  }
}

void loop() {

  pollSlaves();

  for(auto i = 0; i < 2; i++)
    if(greenRequested[i])
    {
      patternChangeTimer = millis() + SWITCH_DELAY / 10;
      greenRequested[i] = false;
    }

  if(millis() > patternChangeTimer)
  {
    currentPattern = (currentPattern + 1) % 4;    // Step the current pattern 
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

    patternChangeTimer = millis() + SWITCH_DELAY;
    //delay(DELAY);                                  // Wait for 2 seconds
    Serial.println("-------");
  }

}