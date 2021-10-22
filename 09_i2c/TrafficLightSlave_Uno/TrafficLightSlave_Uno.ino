/* 
/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - I2C traffic light system : Slave
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

//First slave has ID = 10, second ID is 11
#define SLAVE_ID 11       // The only difference in code for different slaves is slave id
#define LED_RED 3
#define LED_YELLOW 4
#define LED_GREEN 5

#define BUTTON 6

//#define DEBUG 1

const uint8_t leds[] = {LED_RED, LED_YELLOW, LED_GREEN};

// Pattern is: GYR, every bit means correspondent light on or off
// For example 0b001 means Green Off, Yellow Off, Red On
const uint8_t patterns[] = {
  0b001,  //RED index 0
  0b011,  //RED YELLOW index 1
  0b100,  //GREEN index 2
  0b010,  //YELLOW index 3
};

uint8_t currentPatternIndex;
bool greenRequested = false;      // If green was requested, = true
bool greenRequestSent = false;    // True if green request was send to controller

// Master requested data
void requestEvent() {
  uint8_t patternIndexToWrite = currentPatternIndex + 1;        // We trasmit numbers 1 to 4 but internally we use indexes from 0 to 3
  uint8_t packet = patternIndexToWrite                          // We combine current pattern with state of green request
    | (greenRequested && !greenRequestSent ? 0b1111 : 0b0111);
  Wire.write(packet);                                           // And write data to I2C
  #ifdef DEBUG
    Serial.print("#");
    Serial.print(SLAVE_ID);
    Serial.print(": Master requested data: sending ");
    Serial.println(packet, BIN);
  #endif
  #ifndef DEBUG
    Serial.print(".");
  #endif
  if(greenRequested && !greenRequestSent)
    Serial.println("Green request sent");
  greenRequestSent = true;
}

// Master sent to us something
// Master sends just pattern for us to set, so we will just decrease this number by one and set according pattern
// See https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterWriter
void receiveEvent(int howMany)
{
  #ifdef DEBUG
    Serial.print("receiveEvent(");
    Serial.print(howMany);
    Serial.println("):");
  #endif
  while(0 < Wire.available())             // loop through all, most likely it is only one byte
  {
    uint8_t packet = Wire.read();         // receive byte as a uint8_t
    setPattern(packet - 1);               // set the pattern
  }
}

// Set the lights according to pattern index
// Also save the index
void setPattern(uint8_t ptrn)
{
  currentPatternIndex = ptrn;
  uint8_t pattern = patterns[currentPatternIndex];

  if((currentPatternIndex == 2) && greenRequested)
  {
    // If green was requested and now we have green light, reset variables related to that
    greenRequested = false;
    greenRequestSent = false;
    Serial.println("Green granted");
  }

  #ifdef DEBUG
    Serial.print("pattern: ");
    Serial.print(pattern, BIN);
    Serial.print(" Writing ");
  #endif

  // Set the lights
  for(auto i : leds)
  {
    #ifdef DEBUG
      Serial.print(i);
      Serial.print(" << ");
      Serial.print(pattern & 1);
      Serial.print("; ");
    #endif
    digitalWrite(i, pattern & 1);
    pattern = pattern >> 1;
  }

  Serial.println("");
}

void setup() {

  //Start I2C slave and register events
  Wire.begin(SLAVE_ID);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  //Set pin modes
  for(auto i : leds)
    pinMode(i, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  currentPatternIndex = 0;

  Serial.begin(9600);
}

void loop() {
  // Here we just check if green request button was pressed
  if(digitalRead(BUTTON) == LOW)
  {
    if(currentPatternIndex == 0)
      if(!greenRequested)
      {
        greenRequested = true;
        greenRequestSent = false;
        Serial.println("Green requested");
      }
    delay(50);
  }
}
