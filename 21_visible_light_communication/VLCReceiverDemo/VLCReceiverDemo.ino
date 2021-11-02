#define LDR 2

#define STATE_IDLE 0
#define STATE_PREAMBLE 1
#define STATE_DATA 2
#define STATE_EOM 3 //end of message

#define DEBUG 0

#include <avr/sleep.h>

volatile bool lastBit;
bool justAwaken = true;

// INT0 external interrupt - on PIN2
ISR(INT0_vect)
{
  lastBit = (PIND & (1<<PD2));      // See https://www.avrfreaks.net/forum/pin-change-interrupt-low
}

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // put your setup code here, to run once:
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

int c = 0;  //counter to neatly feed the line

unsigned long nextBitTime;
unsigned long nextSampleTime;
unsigned long startTime;

unsigned long bitDelay;
unsigned long sampleDelay;
uint8_t state;
uint8_t currentByte;
uint8_t currentBitNo;

String buffer = "";

uint8_t sample;

inline void setState(uint8_t s)
{
  state = s;
  if(state == STATE_PREAMBLE)
    bitDelay = 50;
  else
    bitDelay = 100;
  sampleDelay = bitDelay / 5;
}

void loop() {

  unsigned long t = millis();

  if(justAwaken)
  {
    #if DEBUG > 0
      Serial.println("Wake up!");
    #endif
    setState(STATE_PREAMBLE);
    justAwaken = false;
    startTime = t;

    nextSampleTime = t;
    nextBitTime = t + bitDelay;
    
    currentBitNo = 0;
    currentByte = 0;
    buffer = "";
  }

  if(t >= nextSampleTime)
  {
    sample = sample + lastBit;
    nextSampleTime = t + sampleDelay;
  }

  if(t >= nextBitTime)
  {
    // Convert sample to bit
    // Put bit into current byte
    uint8_t bit = (sample > 2) ? 1 : 0;
    sample = 0;

    #if DEBUG > 0
      Serial.print(bit);
    #endif

    currentByte = currentByte << 1;
    currentByte = currentByte | bit;
    currentBitNo++;

    if(currentBitNo == 8)
    {
      // Byte is complete.
      #if DEBUG > 0
        Serial.print(" --> [BIN(");
        Serial.print(currentByte, BIN);
        Serial.print(") = '");
        Serial.print((char)currentByte);
        Serial.print("']\tat ");
        Serial.println(t - startTime);
      #endif

      if(state == STATE_DATA)
      {
        if(currentByte == 0)
        {
          Serial.print("\nYou got a message: '");
          Serial.print(buffer);
          Serial.print("' in ");
          Serial.print(t - startTime);
          Serial.println(" msec.");
        }
        else
        {
          //Add current completed byte to the buffer
          buffer += (char)currentByte;
          Serial.print((char)currentByte);
        }
      }

      if((currentByte == 0b10101010) && (state == STATE_PREAMBLE))
      {
        #if DEBUG > 0
          Serial.println("Correct preamble detected");
        #endif
        setState(STATE_DATA);
      }

      if(currentByte == 0)
      {
        #if DEBUG > 0
          Serial.println("Idle detected");
        #endif
        setState(STATE_IDLE);

        while ((UCSR0A & _BV (TXC0)) == 0) {}
        justAwaken = true;
        sleep_enable();
        sleep_cpu();
      }


      currentBitNo = 0;
      currentByte = 0;
    }

    nextBitTime = t + bitDelay;
  }
}
