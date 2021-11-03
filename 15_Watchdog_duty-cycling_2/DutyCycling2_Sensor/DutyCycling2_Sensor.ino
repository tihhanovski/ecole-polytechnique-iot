/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Watchdog based duty-cycling 2
 * Ilja Tihhanovski
 * 
 * 
 */

#include <avr/sleep.h>
#include "DHT.h"

#define LED 12
#define SW 2

#define DHTPIN 8        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

float humidity;
float temperature;
bool measuring = false;                   // We only read it from interrupt, so it should not be volatile
volatile bool violated = false;           // True if time constraint was violated
volatile unsigned long violatedTime = 0;  // Time constraint violation time

DHT dht(DHTPIN, DHTTYPE);                 // Declare sensor

// Wait for transmit buffer to empty. Useful before going to sleep
// see https://forum.arduino.cc/t/how-to-know-when-serial-has-finished-sending/116582/6
inline void waitForSerialToFinish()
{
  while ((UCSR0A & _BV (TXC0)) == 0) {}   
}

void measure()
{
  measuring = true;
  
  delay(random(0, 50));                   // Emulate different times
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (!(isnan(h) || isnan(t))) {
    humidity = h;
    temperature = t;
  }
  else
    Serial.println("fail");

  measuring = false;
}

void output()
{
  Serial.print("humidity: ");
  Serial.println(humidity);
  Serial.print("temperature: ");
  Serial.println(temperature);
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
  pinMode(SW, INPUT_PULLUP);
  digitalWrite(LED, LOW);
  
  // Start serial
  Serial.begin(9600);
  while(!Serial);                                     // Wait for serial to start
}


// Timer interrupt
// If still measuring, show the red light
ISR(TIMER1_COMPA_vect)
{
  if(measuring)
  {
    violatedTime = millis();
    violated = true;
    digitalWrite(LED, HIGH);
  }
}


void loop() {

  violated = false;
  measure();
  output();

  if(violated)
  {
    Serial.print("*** Time constraint violated at ");
    Serial.println(violatedTime);
  }
  Serial.flush ();
  waitForSerialToFinish();

  noInterrupts();               //stop interrupts while configuring
  //TCCR1A = 0;                 // reset timer1 setup registers
  //TCCR1B = 0; 

  // Setup watchdog timer
  WDTCSR |= (1 << WDCE) | (1 << WDE);                 // Enter the setup mode
  WDTCSR = (0<<WDIF)|(0<<WDIE)|(0<<WDCE)|(1<<WDE)     // Set the watchdog to reset processor
    | (1<<WDP3 )|(0<<WDP2 )|(0<<WDP1)|(0<<WDP0);      // after four seconds
    
  interrupts();                                       // Enable interrupts back

  // Duty cycling value
  Serial.print("time: ");
  unsigned long dt = millis();
  Serial.print(dt);
  Serial.print(" / ");
  Serial.print(4000 + dt);
  Serial.print(" = ");
  Serial.println((double)dt / (4000 + dt));
  Serial.println("\n\n");
  waitForSerialToFinish();
  
  // Go to sleep until watchdog wake us
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();

}
