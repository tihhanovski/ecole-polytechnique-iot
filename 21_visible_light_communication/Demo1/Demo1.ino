#define LED 7
#define LDR 8
#define DEMO 1

#include "VLCSender.h"
#include "VLCReceiver.h"

#define SENDER_PERIOD 100

int reading = 0;
int led = 1;
int count = 0;

uint8_t codeSent = 0b00000000; //0b10110111
uint8_t codeRcvd = 0b00000000;
bool byteReceived = false;

VLCSender sender;
VLCReceiver recvr;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);

  sender.begin(LED, SENDER_PERIOD);
  recvr.begin(A0, SENDER_PERIOD, 10, 700);

  Serial.println(codeSent, BIN);
  Serial.flush();
  sender.send(codeSent);

  byteReceived = false;
}


void loop() {

  #ifdef DEMO
  sender.loop();
  recvr.loop();

  if(recvr.enabled && recvr.byteReady && !byteReceived)
  {
    Serial.print("received: ");
    Serial.println(recvr.read(), BIN);
    recvr.stop();
  }

  #endif
  #ifndef DEMO
  
  if (count==10) {
    led = 1-led;
    digitalWrite(LED, led);
    count = 0;
  } 
  else 
  { 
    count++; 
  }

  
  reading = analogRead(A0); //Get 3

  int received = reading > 380 ? 1 : 0;
  
  Serial.print(led);
  Serial.print(" -> ");
  Serial.print(received);
  Serial.print(" #");
  Serial.println(reading);
  delay(50);

  #endif
}
