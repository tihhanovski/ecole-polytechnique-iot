#define LED_RED 2
#define LED_YELLOW 4
#define LED_GREEN 5
#define S1 18
#define PATTERNS_COUNT 4
#define DEBOUNCE_DELAY 50
//#define DEBUG 1

const uint8_t leds[] = {LED_RED, LED_YELLOW, LED_GREEN};

// Pattern is: GYR, every bit means correspondent light on or off
// For example 0b001 means Green Off, Yellow Off, Red On
const uint8_t patterns[] = {
  0b001,  //RED
  0b011,  //YELLOW RED
  0b100,  //GREEN
  0b010,  //YELLOW
};

uint8_t currentPattern, oldPattern;


void setup() {

  for(auto i : leds)
    pinMode(i, OUTPUT);

  currentPattern = 0;
  oldPattern = 255; //impossible pattern

  pinMode(S1, INPUT_PULLUP);

  #ifdef DEBUG
    Serial.begin(9600);
  #endif

}

uint8_t oldButtonState = HIGH;
uint8_t buttonState = HIGH;
unsigned long debounceTimeEnd;

void loop() {

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

}
