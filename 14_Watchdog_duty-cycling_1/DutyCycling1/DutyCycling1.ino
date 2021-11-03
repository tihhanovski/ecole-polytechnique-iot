/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Watchdog based duty-cycling 1 
 * Ilja Tihhanovski
 * 
 * 
 */

#include <avr/wdt.h>
#include <avr/sleep.h>

// We cant use other timers of Arduino because they will not work in SLEEP_MODE_PWR_DOWN

/*
WDTCSR
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


#define LED 12
#define SW 2

float humidity;
float temperature;

inline void waitForSerialToFinish()
{
  // wait for transmit buffer to empty, see https://forum.arduino.cc/t/how-to-know-when-serial-has-finished-sending/116582/6
  while ((UCSR0A & _BV (TXC0)) == 0) {}   
}

void measure()
{
  delay(100);         // Emulate working
  humidity = 60;
  temperature = 20;
}

void output()
{
  Serial.print("humidity: ");
  Serial.println(humidity);
  Serial.print("temperature: ");
  Serial.println(temperature);
  Serial.print("time: ");
  Serial.println(millis());
  Serial.flush ();  
  waitForSerialToFinish();

}

volatile bool measureRequested = false;

void setup() {

  measureRequested = false;
  
  // Setup pins
  pinMode(LED, OUTPUT);
  pinMode(SW, INPUT_PULLUP);
  
  // Start serial
  Serial.begin(9600);
  while(!Serial);                                     // Wait for serial to start
  Serial.println("You can measure now");
  Serial.flush();  
  waitForSerialToFinish();

  // We want to handle FALLING on pin 2
  // see https://tsibrov.blogspot.com/2019/06/arduino-interrupts-part2.html
  // To enable external interrupts bits INT0 and INT1 of EIMSK are used (External Interrupt Mask Register)
  // To control events that fire interrupt, bits 0 to 3 of EICRA are used (External Interrupt Control Register A)
  /* ISC00 and ISC01 control INT0:
   * ISC01  ISC00
   * 0      0     - LOW;
   * 0      1     - CHANGE (e.g. both FALLING and RISING);
   * 1      0     - FALLING;
   * 1      1     - RISING.
   */

  EICRA &= ~(1 << ISC00);                             // Reset ISC00
  EICRA |= (1 << ISC01);                              // Set ISC01
  EIMSK |= (1 << INT0);                               // Enable INT0

  digitalWrite(LED, HIGH);                            // Indicate we are ready for measuring

  set_sleep_mode (SLEEP_MODE_PWR_DOWN);               // Go to sleep until INT0 will wake us up
  sleep_enable();
  sleep_cpu();
}

// INT0 external interrupt - on PIN2
// Actually for this exersize we do not need to do anything here.
// System will just wake up and proceed to do loop
// But in case of system with different tasks, we can use this variable to know what is needed to do
ISR(INT0_vect)
{
  measureRequested = true;  // Just remember that measuring is requested
}

void loop() {
  if(measureRequested)
  {
    measureRequested = false;
    measure();                                          // Measure
    output();                                           // Output data

    // Setup watchdog timer and disable pin 2 interrupt
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
    
    WDTCSR |= (1 << WDCE) | (1 << WDE);                 // Enter the setup mode
    WDTCSR = (0<<WDIF)|(0<<WDIE)|(0<<WDCE)|(1<<WDE)     // Set the watchdog to reset processor
      | (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0);      // after four seconds
    
    EIMSK &= ~(1 << INT0);                              // Disable INT0 - we dont want system to wake up on button press now
    
    interrupts();                                       // Enable interrupts back

    digitalWrite(LED, LOW);                             // Show that measurement is disabled

    // Go to sleep until watchdog wake us
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
  }

}
