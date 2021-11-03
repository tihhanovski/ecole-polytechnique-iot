/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Watchdog based duty-cycling 2
 * Ilja Tihhanovski
 * 
 * 
 */

//#include <avr/wdt.h>
#include <avr/sleep.h>

#define LED 12
#define SW 2

float humidity;
float temperature;
volatile bool measuring = false;

inline void waitForSerialToFinish()
{
  // wait for transmit buffer to empty, see https://forum.arduino.cc/t/how-to-know-when-serial-has-finished-sending/116582/6
  while ((UCSR0A & _BV (TXC0)) == 0) {}   
}

void measure()
{
  measuring = true;
  delay(70);         // Emulate working
  humidity = 60;
  temperature = 20;
  measuring = false;
}

void output()
{
  Serial.print("humidity: ");
  Serial.println(humidity);
  Serial.print("temperature: ");
  Serial.println(temperature);
  Serial.print("time: ");
  unsigned long dt = millis();
  Serial.print(dt);
  Serial.print(" / ");
  Serial.print(4000 + dt);
  Serial.print(" = ");
  Serial.println((double)dt / (4000 + dt));
  //waitForSerialToFinish();
}

void setup() {
  // Setup pins
  pinMode(LED, OUTPUT);
  pinMode(SW, INPUT_PULLUP);
  digitalWrite(LED, LOW);
  
  // Start serial
  Serial.begin(9600);
  while(!Serial);                                     // Wait for serial to start
  Serial.println("Active period started");
  Serial.flush();  
  waitForSerialToFinish();

  // Setup timer to finish measuring
  noInterrupts();             //stop interrupts while configuring
  TCCR1A = 0;                 // reset timer1 setup registers
  TCCR1B = 0; 

  TCNT1 = 0;                  // set internal timer counter 1 to zero
  TCCR1B |= (1 << WGM12);     // enable CTC mode

  OCR1A = 16000;
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);    // setup interrupt on timer compare
  
  interrupts();               //allow interrupts again
  
}

volatile bool violated = false;

// Timer interrupt
// If still measuring, show the red light
ISR(TIMER1_COMPA_vect)
{
  if(measuring)
  {
    violated = true;
    digitalWrite(LED, HIGH);
  }
}


void loop() {
  // put your main code here, to run repeatedly:

  //Serial.print("before ");
  //Serial.println(millis());

  violated = false;
  measure();
  output();

  //Serial.print("after ");
  //Serial.println(millis());

  if(violated)
    Serial.println("time constraint violated");
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

  // Go to sleep until watchdog wake us
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();

}
