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
  /*if (digitalRead(buttonPin) == LOW){
    flipLights();
    delay(50);
  }*/

  if(millis() > logTimer)
  {
    Serial.print("flipCount ");
    Serial.println(flipCount);
    logTimer = millis() + 1000;
  }
}