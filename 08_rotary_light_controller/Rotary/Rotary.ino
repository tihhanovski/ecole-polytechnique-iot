/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - Rotary light controller
 * Ilja Tihhanovski

 * When powering on the Arduino, the LED is off, 
   and turning the rotary controller doesn't do anything - 
   it is in "neutral" state, so to speak.

 * To set the R-value, one must first press the R switch, 
   then turn the shank of the rotary controller to vary the red LED intensity.

 * To set the G-value, one must first press the G switch, 
   then turn the shank of the rotary controller to vary the green LED intensity.

 * To set the B-value, one must first press the B switch, 
   then turn the shank of the rotary controller to vary the blue LED intensity.

 * The RGB values at any given time must be printed in the serial console.

 * Pressing the shank of the rotary controller (remember, it's a switch) 
   will put the rotary controller back in 'N' --neutral--, 
   where turning it no longer changes the intensity of any colour.

 * Finally, an "off switch" of sorts must exist, which sets all of R, G, and B to 0
   (thus, turns off the RGB led), and sets the rotary controller back in 'N'.

*/

#include <Arduino.h>  //mostly to be able to use nullptr in code -- https://arduino.stackexchange.com/questions/34377/setting-a-pointer-to-null-in-arduino-library-error-null-was-not-declared-in

//Define input and output pins
//LED
#define LED_R 3
#define LED_G 5
#define LED_B 6

//Rotary encoder
#define ROTARY_SW   4
#define ROTARY_DT   7
#define ROTARY_CLK  8

//Buttons
#define BUTTON_R 9
#define BUTTON_G 10
#define BUTTON_B 11
#define BUTTON_OFF 12

#define COLOR_STEP 10 // step to encrease or decrease color on every click

//#define DEBUG 1

void setup() {

  //RGB LED pins
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  //Buttons
  pinMode (BUTTON_R, INPUT_PULLUP);
  pinMode (BUTTON_G, INPUT_PULLUP);
  pinMode (BUTTON_B, INPUT_PULLUP);
  pinMode (BUTTON_OFF, INPUT_PULLUP);

  //Rotary encoder
  pinMode(ROTARY_CLK, INPUT);
  pinMode(ROTARY_DT, INPUT);
  pinMode(ROTARY_SW, INPUT_PULLUP);

  //Set all colors to 0
  resetColors();

  //This time serial will be always used
  Serial.begin(9600);
  Serial.flush();

  //Output initial color (rgb = #000000)
  printColor();
}

//RGB values
int red = 0;
int green = 0;
int blue = 0;

//Pointer to current color to set
int * color = nullptr;

//Rotary encoder handling code

//Old saved clk and dt values
int clk = 0;
int dt = 0;

// Get rotation direction from encoder
//  Returns:
//     0 if no direction detected
//    -1 for counter-clockwise direction
//    +1 for clockwise direction
inline short int getDirection()
{
  short int ret = 0;
  //Copy-paste from Moodle:
  int newCLK = digitalRead(ROTARY_CLK);
  int newDT = digitalRead(ROTARY_DT);
  
  for (int i=0; i < 1600; i++) asm volatile ("NOP");  // switch debounce

  if ( (newCLK != clk) && newCLK == LOW ) {           // CLK has changed from HIGH to LOW

    #ifdef DEBUG
      Serial.print(newCLK);
      Serial.print(" ");
      Serial.println(newDT);
    #endif

    if ( dt == LOW)
      ret = -1;
    else
      ret = 1;
  }
  clk = newCLK;
  dt = newDT;
  return ret;
}

// Set all colors to initial value
// Also set current color pointer to "neutral"
inline void resetColors()
{
  red = 0;
  green = 0;
  blue = 0;
  color = nullptr;
}

// Print hex value with leading 0 for values less than F
inline void printHex(int h)
{
  if(h < 16)
    Serial.print("0");
  Serial.print(h, HEX);  
}

// Print out current color (global red, green, blue in hex format)
inline void printColor()
{
  Serial.print("Color: #");
  printHex(red);
  printHex(green);
  printHex(blue);
  Serial.println("");
}

// Set RGB LED pins
// I have common anode LED, so sending 255 to pin means no light (no voltage between anode and color), 
// 0 means brightest light
inline void setRGBLed(byte r, byte g, byte b)
{
  analogWrite(LED_R, 255 - r);
  analogWrite(LED_G, 255 - g);
  analogWrite(LED_B, 255 - b);
}

void loop() {
  // Check color switch buttons
  // When button is pressed, correspondent color will be referenced by our color pointer
  if(digitalRead(BUTTON_R) == LOW)    color = & red;
  if(digitalRead(BUTTON_G) == LOW)    color = & green;
  if(digitalRead(BUTTON_B) == LOW)    color = & blue;

  // If off button is pressed, reset colors
  if(digitalRead(BUTTON_OFF) == LOW)  resetColors();

  // If rotary encoder shaft is pressed, then reset current color pointer
  if(digitalRead(ROTARY_SW) == LOW)   color = nullptr;

  // Receive rotation direction from rotary encoder
  //  0 - no rotation
  // -1 - counter-clockwise
  //  1 - clockwise
  int dir = getDirection();

  if(dir != 0) {                              // If rotation is detected (possible direction is -1 or +1)
    #ifdef DEBUG
      Serial.print("dir: ");
      Serial.println(dir);
      if(dir == 1)
        Serial.println(" + ");
      if(dir == -1)
        Serial.println(" - ");
    #endif
  
    if(color != nullptr) {                    // And if color to change selected (pointer is not NULL)
      *color = *color + dir * COLOR_STEP;     // Increase or decrease it
      if(*color < 0) *color = 0;              // Brightness can't be below zero
      if(*color > 255) *color = 255;          // Brightness can't be above 255

      printColor();                           // If color was changed, output new color
    }
  }

  setRGBLed(red, green, blue);                // Set LEDs on every loop
}