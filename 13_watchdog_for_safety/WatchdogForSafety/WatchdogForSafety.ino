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

  /*
   * Quote from https://forum.arduino.cc/t/what-is-pind/273699
   * That's called direct port manipulation and is used to achieve much higher speed in operating the digital inputs/outputs. 
   * The drawback is that it's very processor dependent and changing the pins is not easy for the casual Arduino programmer.
   * PIND is the input register of port D (pins 0 to 7 on the UNO).
   */
  // See https://www.avrfreaks.net/forum/pin-change-interrupt-low
  // We are using it instead of next (commented out) line
  // (yes, seems that it works here also)
  pressed = !(PIND & (1<<PD2)); 
  //pressed = !digitalRead(SW);                         // To have proper value after reboot  

  Serial.begin(9600);
  while(!Serial);                                     // Wait for serial to start
  Serial.println("Started");

  noInterrupts();                                     // Disable interrupts to avoid messing up the watchdog setup

/*
WDTCSR register bits
  0 - WDP0 : prescaler
  1 - WDP1 : prescaler
  2 - WDP2 : prescaler
  3 - WDE  : System reset on time-out
  4 - WDCE : Change enable. +WDE=1 - 4 clock cycles
  5 - WDP3 : prescaler
  6 - WDIE : Watchdog interrupt enabled
  7 - WDIF : Watchdog interrupt flag

  WDE WDIE  Mode                  Action
  0   0     Stopped               -
  0   1     Interrupt mode        Interrupt
  1   0     System reset mode     Reset
  1   1     Interrupt and reset   Interrupt, then reset
  x   x     System reset mode     Reset

  WDP 3 2 1 0 msec
      0 0 0 0 16
      0 0 0 1 32
      0 0 1 0 64
      0 0 1 1 125
      0 1 0 0 250
      0 1 0 1 500
      0 1 1 0 1000
      0 1 1 1 2000
      1 0 0 0 4000
      1 0 0 1 8000
*/

  // Enter the setup mode, after that we have 4 processor cycles to finish setup
  WDTCSR |= (1 << WDCE) | (1 << WDE);

  // Mnemonics is used, bits is not in actual orders.
  WDTCSR = (0 << WDIF)                                  // Reset interrupt flag
         | (0 << WDIE)                                  // Interrupt not enabled
         | (0 << WDCE)                                  // 
         | (1 << WDE)                                   // System reset on time-out
         | (1 << WDP3)                                  // time
         | (0 << WDP2)                                  // time
         | (0 << WDP1)                                  // time
         | (0 << WDP0);                                 // 1000 corresponds to four seconds


  // Setup interrupt
  // We want to handle both FALLING AND RISING edges
  // see https://tsibrov.blogspot.com/2019/06/arduino-interrupts-part2.html

  // To enable external interrupts bits INT0 and INT1 of EIMSK are used (External Interrupt Mask Register)
  EIMSK |= (1 << INT0);                               // Enable INT0
  
  // To control events that fire interrupt, bits 0 to 3 of EICRA are used (External Interrupt Control Register A)
  /* ISC00 and ISC01 control INT0:
   * 00 - LOW;
   * 01 - CHANGE (e.g. both FALLING and RISING);
   * 10 - FALLING;
   * 11 - RISING.
   */
  EICRA |= (1 << ISC00);                              // Set ISC00
  EICRA &= ~(1 << ISC01);                             // Reset ISC01 (eg put 0 to ISC01 bit of EICRA)
  
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
  pressed = !(PIND & (1<<PD2)); // See more in setup
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
