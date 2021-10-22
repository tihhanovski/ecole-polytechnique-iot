#include <Wire.h>

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

//uint8_t currentPattern, oldPattern;
//uint8_t test;

void requestEvent() {
  //if (digitalRead(DIGITALIN) == HIGH){
    Wire.write("xxxx");
  //} else{
  //  Wire.write("low ");
  //}
}

// from here https://www.arduino.cc/en/Tutorial/LibraryExamples/MasterWriter
void receiveEvent(int howMany)
{
  Serial.print("receiveEvent(");
  Serial.print(howMany);
  Serial.print("): '");
  while(0 < Wire.available())     // loop through all but the last
  {
    int p = Wire.read();         // receive byte as a character
    Serial.print(p);              // print the character
    setPattern(p - 1);
  }
  Serial.println("'");
  //int x = Wire.read();            // receive byte as an integer
  //Serial.println(x);              // print the integer
}

void setPattern(uint8_t currentPattern)
{
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

  Wire.begin(10);
  //Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent); // register event

  for(auto i : leds)
    pinMode(i, OUTPUT);

  //currentPattern = 0;
  //oldPattern = 255; //impossible pattern

  pinMode(S1, INPUT_PULLUP);
  pinMode(9, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  #ifdef DEBUG
    Serial.begin(9600);
  #endif

}

uint8_t oldButtonState = HIGH;
uint8_t buttonState = HIGH;
unsigned long debounceTimeEnd;

void loop() {

  delay(200);

  /*
  digitalWrite(9, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(9, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(1000);
  

  // Debouncing inspired by https://www.arduino.cc/en/Tutorial/BuiltInExamples/Debounce
  uint8_t bs = digitalRead(S1);
  if(oldButtonState != bs)
    debounceTimeEnd = millis() + DEBOUNCE_DELAY;

  if(millis() > debounceTimeEnd)
  {
    if(bs != buttonState)
    {
      buttonState = bs;
      if(buttonState == HIGH)
      {
        currentPattern++;
        if(currentPattern >= PATTERNS_COUNT)
          currentPattern = 0;
      }
    }
  }

  oldButtonState = bs;

  if(currentPattern != oldPattern)
  {
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
    oldPattern = currentPattern;
  }
  */

}
