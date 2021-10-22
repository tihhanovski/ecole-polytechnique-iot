/* 
/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - I2C traffic light system : controller
 * Ilja Tihhanovski

  We will agree on enumerating the different states that the traffic light can have, as follows:
    1. Red
    2. Red-Yellow
    3. Green
    4. Yellow

  So internally we use patterns numbers 0 .. 3,
  But we will transmit numbers 1 to 4 to meet requirements.

  Network protocol is very simple:
  Master send to slave one byte with current pattern number (1 .. 4)
  When master requests the data from slave, slave will return one byte containing
  current pattern number (1 .. 4) and status of green light requested ("pedestrian button" is pressed)

  Data packet consists of one byte with next bits (from lowest):

  #0 - current pattern number
  #1 - current pattern number
  #2 - current pattern number
  #3 - 1 if green light is requested
  bits #4 .. #7 are not used

  For example:

  0b0001 = red light, no green requested (e.g. decimal 1)
  0b0010 = red and yellow light, no green requested (decimal 2)
  0b1001 = red and green requested (decimal 1 & binary 1000)

  0b1100 = yellow light and green is requested - logically impossible cause green could be requested only when red light is on

  Master send to slave only 3 bits (green request bit is aways 0)

*/

#include <Wire.h>

#define SWITCH_DELAY 2000       // Switch patterns every 2 seconds
#define SLAVE_POLL_DELAY 100    // Update slaves state every 0.1 seconds

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
  //Start I2C as master
  Wire.begin();
  Serial.begin(9600);

  //Initialize current pattern and timers to be changed on first loop iteration
  currentPattern = -1;
  patternChangeTimer = 0;
  slavePollTimer = 0;
}

int patterns[2] = {0, 0};           // Here we will save patterns retrieved to compare them later

void pollSlaves()
{
  // Retrieve patterns from slaves
  if(millis() > slavePollTimer) // Only when time is come
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

    slavePollTimer = millis() + SLAVE_POLL_DELAY;  // Set new timer
  }
}

void loop() {

  pollSlaves();

  //If green was requested by somebody, cut the timer
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
    patternChangeTimer = millis() + SWITCH_DELAY; // Now wait for 2 seconds before next switch
  }

}