/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - Serial
 * Ilja Tihhanovski
 * 
 * Main functionality implemented in method binaryOutput is to show given value bits using LED-s
 * I used same electrical scheme and just enhanced the code to explore possibilities offered by Serial
 * 
 * This code based on example from Arduino:
 * https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
 */

// it is possible to use another pins or use more pins changing pins array contents and/or pinsCount
unsigned short int pins[] = {13, 12, 11, 10, 9, 8};   // Array of pins numbers
unsigned short int pinsCount = 6;                     // Count of pins used
unsigned short int value;                             // value we will show
unsigned short int incoming = 0;                      // Last byte received from Serial

#define MODE_AUTOMATIC 0                              // Just like previous hand-in
#define MODE_SERIAL_INPUT 1                           // Wait for input and output bytes to LED-s

unsigned short mode = MODE_AUTOMATIC;                 // Current operation mode

void setup() {
  
  for(unsigned short int i = 0; i < pinsCount; i++)   // Initialize pins as an output.
    pinMode(pins[i], OUTPUT);

  value = 0;                                          // Initialize value we will output.

  Serial.begin(9600);                                 // Start serial for debugging reasons
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

//check if we need to switch mode
void checkSwitchMode(unsigned short int b)
{
  if(b == 'M')
  {
    if(mode == MODE_AUTOMATIC)
    {
      mode = MODE_SERIAL_INPUT;
      outputManualModeMessage();
    }
    else
      mode = MODE_AUTOMATIC;
  }

}

void outputManualModeMessage()
{
  Serial.println("Manual mode. Enter something to output, enter M to switch mode");
}

void loop() {
  if(mode == MODE_AUTOMATIC)                            // if current mode is automatic
  {
    Serial.print("Automatic mode, value: ");
    Serial.print(value);                                // output value to serial
    Serial.println("\tEnter M to switch mode");
    binaryOutput(value);                                // set LEDs according to value
    delay(200);                                         // wait for 0.2 seconds
    value++;                                            // increment value
    if(value >= 64)                                     // if value is too big to display (2*6 -1)
      value = 0;

    if (Serial.available() > 0) {                       // check if mode needs to be switched
      incoming = Serial.read();
      checkSwitchMode(incoming);
    }
  }

  // We read only one byte at once to make loop method take as little time as possible.
  // (in case of many bytes sent to Arduino, next byte will be read during "next loop method iteration")
  // Serial port operations may be time consuming, so in case we want to do something "real-time"
  // we should avoid long loops.
  // One possibility is to avoid loops (for, while) inside loop method.
  if(mode == MODE_SERIAL_INPUT)                         // manual mode:
  {
    if (Serial.available() > 0) {                       // if we got something from Serial
      incoming = Serial.read();                         // read next byte
      if(incoming != 10)                                // ignore LF
      {
        Serial.print("Entered ");                       // output entered value
        Serial.println(incoming, DEC);
        binaryOutput(incoming);
        checkSwitchMode(incoming);                      // check if mode needs to be switched
        delay(100);                                     // wait for 0.1 seconds
      }
    }
  }
}
