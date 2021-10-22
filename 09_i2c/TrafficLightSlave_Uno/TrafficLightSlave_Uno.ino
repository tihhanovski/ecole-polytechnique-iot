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

bool greenRequested = false;
bool greenRequestSent = false;

void requestEvent() {
  uint8_t patternIndexToWrite = currentPatternIndex + 1;
  uint8_t packet = patternIndexToWrite
    | (greenRequested && !greenRequestSent ? 0b1111 : 0b0111);
  Wire.write(packet);
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


// from here https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterWriter
void receiveEvent(int howMany)
{
  #ifdef DEBUG
    Serial.print("receiveEvent(");
    Serial.print(howMany);
    Serial.println("):");
  #endif
  while(0 < Wire.available())             // loop through all
  {
    uint8_t packet = Wire.read();         // receive byte as a uint8_t
    setPattern(packet - 1);               // set the pattern
  }
}

void setPattern(uint8_t ptrn)
{
  currentPatternIndex = ptrn;
  uint8_t pattern = patterns[currentPatternIndex];

  if((currentPatternIndex == 2) && greenRequested)
  {
    greenRequested = false;
    greenRequestSent = false;
    Serial.println("Green granted");
  }

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

  Serial.println("");
}

void setup() {

  //Start I2C slave and register events
  Wire.begin(SLAVE_ID);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  for(auto i : leds)
    pinMode(i, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);

  currentPatternIndex = 0;

  Serial.begin(9600);
}

void loop() {
  if(digitalRead(BUTTON) == LOW)
  {
    //Serial.println("*************");
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
