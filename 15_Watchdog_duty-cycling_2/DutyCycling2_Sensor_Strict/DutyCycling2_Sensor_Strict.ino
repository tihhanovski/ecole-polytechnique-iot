/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Watchdog based duty-cycling 2
 * Ilja Tihhanovski
 * 
 * Strict version of duty-cycling - system will shut down 
 * in the middle of "measurement" if it takes too long.
 * 
 * Unfortunately it is impossible to output to Serial during interrupt handler
 * https://arduino.stackexchange.com/questions/46595/sending-serial-data-in-an-interrupt
 * 
 */

#include <avr/sleep.h>
#include "DHT.h"

#define LED 12

#define DHTPIN 8        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

float humidity;
float temperature;
DHT dht(DHTPIN, DHTTYPE);                 // Declare sensor

// Wait for transmit buffer to empty. Useful before going to sleep
// see https://forum.arduino.cc/t/how-to-know-when-serial-has-finished-sending/116582/6
inline void waitForSerialToFinish() {
  while ((UCSR0A & _BV (TXC0)) == 0) {}   
}

void measure() {
  
  delay(random(0, 20));                   // Emulate different times

  float h = dht.readHumidity();           // Get data from the sensor
  float t = dht.readTemperature();
  
  if (!(isnan(h) || isnan(t))) {
    humidity = h;
    temperature = t;
  }
  else
    Serial.println("fail");

}

void output() {
  Serial.print("t: "); Serial.println(humidity);
  Serial.print("h: "); Serial.println(temperature);
}

void setup() {
  // Setup timer to know if time constraint was violated
  noInterrupts();             //stop interrupts while configuring
  TCCR1A = 0;                 // reset timer1 setup registers
  TCCR1B = 0;

  TCNT1 = 0;                  // set internal timer counter 1 to zero
  TCCR1B |= (1 << WGM12);     // enable CTC mode

  OCR1A = 16000;
  TCCR1B &= ~(1 << CS12);
  TCCR1B |= (1 << CS11) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);    // setup interrupt on timer compare
  
  interrupts();               //allow interrupts again
  
  randomSeed(analogRead(0));  // For our different time emulation purposes
  dht.begin();                // Start temperature and humidity sensor

  // Setup pins
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);     // Switch the LED off if it was on (time constraint violated during previous cycle)
  
  Serial.begin(9600);         // Start serial
  while(!Serial) {}           // Wait for serial to start
  Serial.println("\n\n");
}

inline void prepareWatchdog()
{
  WDTCSR |= (1 << WDCE) | (1 << WDE);                 // Enter the setup mode
  WDTCSR = (0<<WDIF)|(0<<WDIE)|(0<<WDCE)|(1<<WDE)     // Set the watchdog to reset processor
    | (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0);      // after four seconds      
}

inline void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();  
}

inline void outputTimingInformation()
{
  Serial.print("dt: ");
  unsigned long dt = millis();
  double c = 100 * (double)dt / (4000 + dt);
  Serial.println(c);                             // time 
}


// Timer interrupt
// Show the red light, setup watchdog and go to sleep
ISR(TIMER1_COMPA_vect) {
  digitalWrite(LED, HIGH);
  prepareWatchdog();
  goToSleep();
}


void loop() {
  measure();
  output();

  TCCR1A = 0;                             // reset timer1 to prevent reset during output of timing information
  TCCR1B = 0;
  noInterrupts();                         //stop interrupts while configuring
  prepareWatchdog();
  interrupts();                           // Enable interrupts back

  outputTimingInformation();              // Print duty cycle value
  waitForSerialToFinish();  
  goToSleep();
}
