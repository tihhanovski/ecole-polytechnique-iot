#include <Wire.h>

#define SLAVE_ID 10
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

uint8_t currentPattern;

void requestEvent() {
  Wire.write(currentPattern);
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
    //Serial.print(p);                 // print the pattern received
    setPattern(p - 1);               // set the pattern
  }
  //Serial.println("'");
}

void setPattern(uint8_t ptrn)
{
  currentPattern = ptrn;
  uint8_t pattern = patterns[currentPattern];

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

  currentPattern = 0;

  #ifdef DEBUG
    Serial.begin(9600);
  #endif

}

void loop() {
  delay(200);
}
