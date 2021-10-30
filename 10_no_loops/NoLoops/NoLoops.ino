/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * No Loops
 * Ilja Tihhanovski
 */

#define IR_PIN 2
#define PIN_COUNT 4 // number of buttons and leds

// Define pins for LEDs and buttons
// Each button (except last) is connected to own pin and to pin 2 (to fire INT0)
// Diods are used to prevent connecting other buttons to ground when some button is pressed
// (see electrical scheme)
// Last button connected only to pin 2 (I had only 3 diods, so I had to figure that out)
const uint8_t leds[PIN_COUNT] = {3, 4, 5, 6};
const uint8_t buttons[PIN_COUNT] = {8, 9, 10, IR_PIN};

// Which led is on=true or off=false
volatile bool lightOn[PIN_COUNT] = {false, false, false, false};

void setup()
{
  // Set pinmode for leds and switch the leds off (just in case)
  for(auto led : leds)
  {
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
  }

  // Set pinmodes for buttons
  for(auto button : buttons)
    pinMode(button, INPUT_PULLUP);

  // Setup interrupt for pin 2
  attachInterrupt(digitalPinToInterrupt(IR_PIN), flip, FALLING );
}

// Interrupt handler
// Fired when pin 2 is falling from HIGH to LOW
void flip()
{
  for(uint8_t i = 0; i < PIN_COUNT; i++)    // We check for every pin (0 to 3)
    if(!digitalRead(buttons[i]))            // If the pin is LOW
    {
      lightOn[i] = !lightOn[i];             // Flip the light
      digitalWrite(leds[i], lightOn[i]);    // Set HIGH or LOW for the pin
      return;                               // Exit
      // We should exit after first pin in LOW state found to prevent flipping
      // of the last led which corresponds to last button connected only to pin 2
      // When no other pin is low, we have to assume, that last button is pressed
    }
}

// To keep code as simple as possibe, nothing is in the loop
// Actually it could be better if we put here digitalWrite-s?
void loop() {}
