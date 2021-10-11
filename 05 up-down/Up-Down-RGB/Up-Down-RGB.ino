/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - What Goes Up, Must Come Down
 * Ilja Tihhanovski
 * 
 */

// Define pins for input and output
#define BTN_RED 7
#define BTN_GREEN 6
#define BTN_BLUE 5
#define BTN_OFF 4

#define LED_RED 11
#define LED_GREEN 10
#define LED_BLUE 9

// Delay to make oscillation slower a little bit
#define DELAY 5

// Uncomment next define to use Serial debug (and have bloated program :))
//#define DEBUG

byte lastButtonPressed;     // last button
byte brightness;            // current brigthness
byte bdelta;                // brightness delta - we will increase/decrease brightness by 1 every step

void setup() {

  // Set up pins for buttons to be input pullup
  pinMode(BTN_RED, INPUT_PULLUP);
  pinMode(BTN_GREEN, INPUT_PULLUP);
  pinMode(BTN_BLUE, INPUT_PULLUP);
  pinMode(BTN_OFF, INPUT_PULLUP);

  // Set up pins for LED (actually we could just comment it out to get extra 24 bytes of storage space
  // At least on Arduino UNO it will work without explicit setup of output pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // By default lights will be off
  lastButtonPressed = BTN_OFF;
  // I have LED with common anode, so brightness is inverted (255 means lights off, 0 - full lights)
  // At first we will decrease our inverted brightness :)
  // To use common cathode LED, default brightness should be 0 and bdelta = 1
  brightness = 255;
  bdelta = -1; 

  //debug stuff
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
}

// Check if button is pressed and save last pressed button
// Argument c - pin to check
void checkPress(byte c)
{
  #ifdef DEBUG
    if(digitalRead(c) == LOW)
      if(lastButtonPressed != c)
      {
        switch(c)
        {
          case BTN_RED:
            lastButtonPressed = c;
            Serial.print("Red");
            break;
          case BTN_GREEN:
            lastButtonPressed = c;
            Serial.print("Green");
            break;
          case BTN_BLUE:
            lastButtonPressed = c;
            Serial.print("Blue");
            break;
          case BTN_OFF:
            lastButtonPressed = c;
            Serial.print("Off");
            break;
        }
        Serial.println("Switch pressed");
      }
  #else
    // Without debug output code can be quite small and elegant :)
    // We could use if and OR (||) instead of switch, but switch saves for us two extra bytes
    if(digitalRead(c) == LOW)
      switch(c)
      {
        case BTN_RED: 
        case BTN_GREEN: 
        case BTN_BLUE: 
        case BTN_OFF:
          lastButtonPressed = c;
          //break; -- break here is not needed?
      }
  #endif
}

void loop() {
  //check if button is pressed
  checkPress(BTN_RED);
  checkPress(BTN_GREEN);
  checkPress(BTN_BLUE);
  checkPress(BTN_OFF);

  // If R, G or B button was pressed, set brightness of corresponding pin.
  // Otherwise set corresponding pin to 255 (that means switch off for common anode RGB LED)
  analogWrite(LED_RED, lastButtonPressed == BTN_RED ? brightness : 255);
  analogWrite(LED_GREEN, lastButtonPressed == BTN_GREEN ? brightness : 255);
  analogWrite(LED_BLUE, lastButtonPressed == BTN_BLUE ? brightness : 255);

  //If LED is on, oscillate
  if(lastButtonPressed != BTN_OFF)
  {
    brightness = brightness + bdelta;
    if(brightness == 0 || brightness == 255)
      bdelta = -bdelta;
    delay(DELAY); //Wait a little
  }
}
