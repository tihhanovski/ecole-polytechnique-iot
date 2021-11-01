/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * A watchdog for safety
 * Ilja Tihhanovski
 * 
 * Some extra information found at:
 * Watchdog setup:  https://www.avrfreaks.net/forum/watchdog-enable
 * Watchdog reset:  https://github.com/vancegroup-mirrors/avr-libc/blob/master/avr-libc/include/avr/wdt.h#L99
 * Interrupts:      https://tsibrov.blogspot.com/2019/06/arduino-interrupts-part2.html
 *                  https://www.avrfreaks.net/forum/pin-change-interrupt-low
 * 
 */

#define LED 12
#define SW 2

volatile bool pressed = false;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(SW, INPUT_PULLUP);

  pressed = !digitalRead(SW);                         // To have proper value after reboot  

  Serial.begin(9600);
  while(!Serial);                                     // Wait for serial to start
  Serial.println("Started");

  noInterrupts();                                     // Disable interrupts to avoid messing up the watchdog setup

  WDTCSR |= (1 << WDCE) | (1 << WDE);                 // Enter the setup mode
  WDTCSR = (0<<WDIF)|(0<<WDIE)|(0<<WDCE)|(1<<WDE)     // Set the watchdog to reset processor
    | (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0);      // after four seconds

  //We want to handle both FALLING AND RISING edges
  //see https://tsibrov.blogspot.com/2019/06/arduino-interrupts-part2.html
  EICRA |= (1 << ISC00);                              // Set ISC00
  EICRA &= ~(1 << ISC01);                             // Reset ISC01
  EIMSK |= (1 << INT0);                               // Enable INT0
  
  interrupts();                                       // Enable interrupts back

}

/*
ISR(WDT_vect)   //watchdog interrupt
{
  //Nothing to do here, we just reset
}
*/

// INT0 external interrupt - on PIN2
ISR(INT0_vect)
{
  // See https://www.avrfreaks.net/forum/pin-change-interrupt-low
  pressed = !(PIND & (1<<PD2)); // We are using it instead of next (commented out) line
  //pressed = !digitalRead(SW);
}

// LED switching - we want to do digitalWrite only when it is really needed
bool ledOn = false;
inline void switchLed(bool b)
{
  if(ledOn != b)
    digitalWrite(LED, ledOn = b);
}

int cnt = 0;
void loop() {
  if(pressed)
    switchLed(true);                              // Switch the led on (if it was off)
  else
  {
    switchLed(false);                             // Switch the led off (if it was on)
    delay(100);
    Serial.print((++cnt %= 50) ? "." : "\n.");    // Print lines of 50 dots per line
    asm("wdr");                                   // This is what wdt_reset do, see https://github.com/vancegroup-mirrors/avr-libc/blob/master/avr-libc/include/avr/wdt.h#L99
  }
}
