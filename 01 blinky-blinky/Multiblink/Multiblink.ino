/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - blinky-blinky - Hand-In - 2pt Assignment
 * Ilja Tihhanovski
 * 
 * Main functionality implemented in method binaryOutput is to show given value bits using LED-s
 * Method could be used as shown here (simple example with incremented values) or to display random values etc.
 * 
 * This code based on example from Arduino:
 * https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
 */

// it is possible to use another pins or use more pins changing pins array contents and/or pinsCount
unsigned short int pins[] = {13, 12, 11, 10, 9, 8};   // Array of pins numbers
unsigned short int pinsCount = 6;                     // Count of pins used
unsigned short int value;                             // value we will show

void setup() {
  
  for(unsigned short int i = 0; i < pinsCount; i++)   // Initialize pins as an output.
    pinMode(pins[i], OUTPUT);

  value = 0;                                          // Initialize value we will output.

  //Serial.begin(9600);                                 // Start serial for debugging reasons
}

//Switches LEDs according to bits in argument v
void binaryOutput(unsigned short int v)
{
  for(unsigned short int i = 0; i < pinsCount; i++)
  {
    if(v & 1)                                         // bitwise compare value with 1, eg is lowest bit in v set
      digitalWrite(pins[i], HIGH);                    // if bit = 1, turn the LED on
    else
      digitalWrite(pins[i], LOW);                     // if bit != 1, turn the LED off
    v = v >> 1;                                       // shift value one bit to the right, so at next iteration we will check next bit
  }
}


void loop() {
  //Serial.println(value);                              // output value to serial
  binaryOutput(value);                                      // set LEDs according to value
  //delay(100);                                         // wait for 0.1 seconds
  value++;                                            // increment value
  if(value >= 64)                                     // if value is too big to display (2*6 -1)
    value = 0;
}
