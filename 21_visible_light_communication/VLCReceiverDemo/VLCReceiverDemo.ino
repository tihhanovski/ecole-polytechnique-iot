/* 
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Visible light communication
 * Ilja Tihhanovski
 *
 * Receiver with synchronization
 */


#define LDR 2                   // Input pin
#define BIT_DELAY 5000          // In microseconds

// Receiver states:
#define STATE_IDLE 0
#define STATE_PREAMBLE 1
#define STATE_DATA 2

#define DEBUG 0

#include <avr/sleep.h> 

volatile bool lastBit;      // Last bit receiver got
bool justAwaken = true;

// INT0 external interrupt - on PIN2
ISR(INT0_vect)
{
  lastBit = (PIND & (1<<PD2));      // See https://www.avrfreaks.net/forum/pin-change-interrupt-low
}

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);        // Setup sleep mode
  pinMode(LDR, INPUT);

  Serial.begin(9600);

  // Setup interrupt
  noInterrupts();

  //We want to handle both FALLING AND RISING edges
  //see https://tsibrov.blogspot.com/2019/06/arduino-interrupts-part2.html
  EICRA |= (1 << ISC00);                              // Set ISC00
  EICRA &= ~(1 << ISC01);                             // Reset ISC01
  EIMSK |= (1 << INT0);                               // Enable INT0

  interrupts();

  // Go to sleep
  sleep_enable();
  sleep_cpu();
}

unsigned long nextBitTime;        // Time to start next bit
unsigned long nextSampleTime;     // Time to measure next sample (we take 5 samples per bit)
unsigned long startTime;          // Message start time

unsigned long bitDelay;           // Bit duration in microseconds
unsigned long sampleDelay;        // Time between samples in microseconds
uint8_t state;                    // Current receiver state
uint8_t currentByte;              // Byte buffer
uint8_t currentBitNo;             // Currently received bit number 

String buffer = "";               // Message (packet) buffer

uint8_t sample;                   // Samples sum

// Sets current state
// For preamble state we use 2x shorter delays (preamble is 10101010 sent in double speed)
inline void setState(uint8_t s)
{
  state = s;
  if(state == STATE_PREAMBLE)
    bitDelay = BIT_DELAY / 2;
  else
    bitDelay = BIT_DELAY;
  sampleDelay = bitDelay / 5;             // Five samples per bit
}

void loop() {

  unsigned long t = micros();

  if(justAwaken)                          // Something is incoming (CHANGE on input pin wake microcontroller up)
  {
    #if DEBUG > 0
      Serial.println("Wake up!");
    #endif
    setState(STATE_PREAMBLE);             // Expecting preamble
    justAwaken = false;
    startTime = t;                        // Save the message start time

    nextSampleTime = t;                   // Prepare to take first sample
    nextBitTime = t + bitDelay;           // Schedule the next bit start
    
    currentBitNo = 0;                     // Initialize buffers
    currentByte = 0;
    buffer = "";
  }

  if(t >= nextSampleTime)                 // Next sample must be taken
  {
    sample = sample + lastBit;            // Accumulate samples
    nextSampleTime = t + sampleDelay;     // Schedule next sample time
  }

  if(t >= nextBitTime)                    // Its time to move on to the next bit, so we must save the last
  {
    // Convert sample to bit
    // Put bit into current byte
    uint8_t bit = (sample > 2) ? 1 : 0;
    sample = 0;                           // Reset the samples accumulator

    #if DEBUG > 0
      Serial.print(bit);
    #endif

    currentByte = currentByte << 1;       // Shift the byte buffer
    currentByte = currentByte | bit;      // Put previous bit into byte
    currentBitNo++;                       // Increase bit counter

    if(currentBitNo == 8)                 // If all 8 bits is read
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
        // If state is DATA and we receive 0, then we know that the message/packet is over
        // Output the message/packet
        if(currentByte == 0)
        {
          Serial.print("\t// in ");
          Serial.print(t - startTime);
          Serial.print(" usec at speed ");
          Serial.print((double)(buffer.length()) / (t - startTime) * 1000000 );
          Serial.println(" bps");
        }
        else
        {
          // We received data byte
          // Add current completed byte to the buffer
          buffer += (char)currentByte;
          Serial.print((char)currentByte);
        }
      }

      if((currentByte == 0b10101010) && (state == STATE_PREAMBLE))
      {
        // Currect preamble received, now expect data bytes 
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
        setState(STATE_IDLE);                     // Switch to the idle state

        while ((UCSR0A & _BV (TXC0)) == 0) {}     // Wait for serial to finish
        justAwaken = true;
        sleep_enable();                           // Go to sleep
        sleep_cpu();
      }

      // Reset the byte buffer after each byte
      currentBitNo = 0;
      currentByte = 0;
    }

    nextBitTime = t + bitDelay;     // Schedule the next bit time
  }
}
