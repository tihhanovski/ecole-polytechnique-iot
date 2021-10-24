#define LED_BUTTON_DELTA 5
#define IR_PIN 2
#define PIN_COUNT 4
//#define NO_PIN

const uint8_t leds[PIN_COUNT] = {3, 4, 5, 6};
const uint8_t buttons[PIN_COUNT] = {8, 9, 10, IR_PIN};

volatile bool lightOn[PIN_COUNT] = {false, false, false, false};

/*
uint8_t led2button(uint8_t led)
{
  return led + LED_BUTTON_DELTA;
}

uint8_t button2led(uint8_t button)
{
  return button - LED_BUTTON_DELTA;
}
*/

void setup()
{
  for(auto led : leds)
  {
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
  }

  for(auto button : buttons)
    pinMode(button, INPUT_PULLUP);

  //pinMode(IR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IR_PIN), flip, FALLING );
}

void flip()
{
  //bool switched = false;
  for(uint8_t i = 0; i < PIN_COUNT; i++)
    if(!digitalRead(buttons[i]))
    {
      lightOn[i] = !lightOn[i];
      digitalWrite(leds[i], lightOn[i]);
      return;
    }

    //lightOn[3] = !lightOn[3];
    //digitalWrite(leds[3], lightOn[3]);
}

void loop()
{
  //for(auto led : leds)
  //  digitalWrite(led, (!digitalRead(IR_PIN)) && (!digitalRead(led2button(led))));
}


/*
#define buttonPin 2
#define redLEDPin 7
#define greenLEDPin 8

bool LEDOn = false;
volatile long int flipCount = 0;
long logTimer = 0;

void flipLights() {

  flipCount++;

  if (LEDOn) {
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, HIGH);
  } else {
    digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, LOW);
  }
  LEDOn = !LEDOn;

}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  
  digitalWrite(redLEDPin, LOW);
  digitalWrite(greenLEDPin, LOW);

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(buttonPin), flipLights, FALLING );
}

void loop() {
  //if (digitalRead(buttonPin) == LOW){
  //  flipLights();
  //  delay(50);
  //}

  if(millis() > logTimer)
  {
    Serial.print("flipCount ");
    Serial.println(flipCount);
    logTimer = millis() + 1000;
  }
}
*/