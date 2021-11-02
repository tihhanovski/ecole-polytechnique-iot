#define LED 7
#define BTN 6

#include "VLCSender.h"

#define SENDER_PERIOD 100

uint8_t codeSent = 0b00000000; //0b10110111

VLCSender sender;

void setup() {

  pinMode(BTN, INPUT_PULLUP);

  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);

  sender.begin(LED, SENDER_PERIOD);

  Serial.println("Press the button to send");
  Serial.flush();
  //sender.send(codeSent);
}

bool pressed;

void loop() {
  if(digitalRead(BTN) == LOW)
  {
    Serial.println("click");
    sender.printSync("Hello, Arduino! "); // bug in receiver? last byte is always messed up
    delay(2000);
  }
  delay(200);
  //sender.loop();
}
