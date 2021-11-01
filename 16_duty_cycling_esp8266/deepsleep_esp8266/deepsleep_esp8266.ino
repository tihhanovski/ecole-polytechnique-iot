/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Duty-cycling and deep sleep in ESP8266
 * Ilja Tihhanovski

  Current draw:
    With deep sleep:
      sleeping: 9mA
      blinking: 26.5mA
    With delays
      waiting: 24.3mA
      blinking: 26.5mA
*/

#define WITH_SLEEPS             // I wrote two versions of code (with or without sleepmode) to compare power consumption
#define LED 5 // 5 is D1        
#define BLINK_CYCLES 5          // How many times to blink
#define SLEEP_TIME 5e6          // How long should ESP8266 sleep (in microseconds)
#define BLINK_DELAY 100         // Delay between blinks (in milliseconds)
#define MESSAGE "I am awake!"

#ifdef WITH_SLEEPS

void setup() {
  pinMode(LED, OUTPUT);         
  Serial.begin(9600);

  Serial.println(MESSAGE);      // Output message

  // Blink
  for(auto i = 0; i < BLINK_CYCLES; i++)
  {
    digitalWrite(LED, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(LED, LOW);
    delay(BLINK_DELAY);
  }

  // Go to sleep
  ESP.deepSleep(SLEEP_TIME);
}

void loop() {}  // Nothing to loop here

#endif

//======================= just to compare - code without sleep ===============
#ifndef WITH_SLEEPS

void setup() {
   pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println("I am awake!");
  for(auto i = 0; i < BLINK_CYCLES; i++)
  {
    digitalWrite(LED, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(LED, LOW);
    delay(BLINK_DELAY);
  }
  delay(5000);

}

#endif
