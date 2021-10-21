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

#include <Wire.h>

#define SLAVE_ID 11       // Only difference in code is slave id
#define LED_RED 2
#define LED_YELLOW 3
#define LED_GREEN 4

#define DEBUG 1

const uint8_t leds[] = {LED_RED, LED_YELLOW, LED_GREEN};

// Pattern is: GYR, every bit means correspondent light on or off
// For example 0b001 means Green Off, Yellow Off, Red On
const uint8_t patterns[] = {
  0b001,  //RED
  0b011,  //RED YELLOW 
  0b100,  //GREEN
  0b010,  //YELLOW
};

uint8_t currentPatternIndex;

bool greenRequested = false;

void requestEvent() {
  Wire.write(currentPatternIndex + 1);
}


// from here https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterWriter
void receiveEvent(int howMany)
{
  Serial.print("receiveEvent(");
  Serial.print(howMany);
  Serial.println("):");
  while(0 < Wire.available())     // loop through all
  {
    uint8_t p = Wire.read();         // receive byte as a uint8_t
    setPattern(p - 1);               // set the pattern
  }
}

void setPattern(uint8_t ptrn)
{
  currentPatternIndex = ptrn;
  uint8_t pattern = patterns[currentPatternIndex];

  #ifdef DEBUG
    Serial.print("pattern: ");
    Serial.print(pattern, BIN);
    Serial.print(" Writing ");
  #endif

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

  #ifdef DEBUG
    Serial.println("");
  #endif
}

void setup() {

  //Start I2C slave and register events
  Wire.begin(SLAVE_ID);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  for(auto i : leds)
    pinMode(i, OUTPUT);

  currentPatternIndex = 0;

  #ifdef DEBUG
    Serial.begin(9600);
  #endif
}

void loop() {
  delay(200);
}
