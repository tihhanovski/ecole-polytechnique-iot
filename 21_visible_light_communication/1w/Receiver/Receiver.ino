#include <avr/sleep.h>

#define LDR 2

#define STATE_IDLE 0
#define STATE_INCOMING 1
#define DEBUG 1

#define DELAY_SCALE   3
#define DELAY_START   DELAY_SCALE * 7
#define DELAY_FINISH  DELAY_SCALE * 13
#define DELAY_1       DELAY_SCALE / 2
#define DELAY_0       DELAY_SCALE * 2
#define DELAY_PAUSE   DELAY_SCALE * 3

volatile bool started = false;
volatile unsigned long startedTime = 0;
volatile unsigned long impulsLength = 0;
volatile unsigned long impulsNo = -1;

ISR(INT0_vect)
{
  unsigned long t = millis();
  bool ledOn = (PIND & (1<<PD2));      // See https://www.avrfreaks.net/forum/pin-change-interrupt-low
  if(ledOn && !started)
  {
    started = true;
    startedTime = t;
    impulsLength = 0;
  }

  if(started && !ledOn)
  {
    impulsLength = t - startedTime;
    started = false;
    impulsNo++;
  }
}

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  pinMode(LDR, INPUT);

  Serial.begin(9600);

  noInterrupts();

  //We want to handle both FALLING AND RISING edges
  //see https://tsibrov.blogspot.com/2019/06/arduino-interrupts-part2.html
  EICRA |= (1 << ISC00);                              // Set ISC00
  EICRA &= ~(1 << ISC01);                             // Reset ISC01
  EIMSK |= (1 << INT0);                               // Enable INT0

  interrupts();

  sleep_enable();
  sleep_cpu();
}

unsigned long processedImpulsNo = 0;
uint8_t receiverState = STATE_IDLE;

String buffer = "";
uint8_t currentByte;
uint8_t currentBit;

void startByte()
{
  currentByte = 0;
  currentBit = 0;
}

void addByte()
{
  buffer = buffer + (char)currentByte;
  startByte();  
}

void addBit(bool b)
{
  if(receiverState == STATE_INCOMING)
  {
    #if DEBUG > 1
      Serial.print("\t");
      Serial.print(b);
    #endif

    if(b)
      currentByte |= 1 << (7 - currentBit);
    currentBit++;

    #if DEBUG > 1
      Serial.print("\t --> ");
      Serial.print(currentByte, BIN);
    #endif

    if(currentBit > 7)
    {
      #if DEBUG > 1
        Serial.print("\t --> ");  
      #endif
      #if DEBUG > 0
        Serial.print((char)currentByte);
      #endif
      addByte();
    }
  }
  #if DEBUG > 1
    Serial.println("");
  #endif
}

void loop() {
  if(processedImpulsNo != impulsNo)
  {
    #if DEBUG > 1
      Serial.print(impulsNo);
      Serial.print(":\t");
      Serial.print(impulsLength);
      Serial.print("\t");
    #endif
    processedImpulsNo = impulsNo;

    //Decode impuls length:

    uint8_t impulsMeaning = 0;

    if(impulsLength > DELAY_FINISH)
    {
      //Serial.println("FINISH");
      receiverState = STATE_IDLE;
      //TODO output message
      Serial.print("\nMessage: '");
      Serial.print(buffer);
      Serial.println("'");

      buffer = "";
      startByte();
      impulsNo = 0;
      processedImpulsNo = 0;

      while ((UCSR0A & _BV (TXC0)) == 0) {} // wait for serial to finish
      sleep_enable();
      sleep_cpu();
    }
    else if(impulsLength > DELAY_START)
    {
      //Serial.println("START");
      //TODO init buffer
      receiverState = STATE_INCOMING;
      buffer = "";
      startByte();
    }
    else if (impulsLength > DELAY_0)
      addBit(0);
    else if (impulsLength > DELAY_1)
      addBit(1);
    //else
    //  Serial.println("GARBAGE");

  }

}
