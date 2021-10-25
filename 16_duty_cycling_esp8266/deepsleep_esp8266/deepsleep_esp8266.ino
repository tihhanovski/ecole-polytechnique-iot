/*

  Current draw:
    With deep sleep:
      sleeping: 9mA
      blinking: 26.5mA
    With delays
      waiting: 24.3mA
      blinking: 26.5mA
*/

#define WITH_SLEEPS
#define LED 5 // 5 is D1
#define BLINK_CYCLES 5
#define SLEEP_TIME 5e6
#define BLINK_DELAY 100
#define MESSAGE "I am awake!"

#ifdef WITH_SLEEPS

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println(MESSAGE);

  for(auto i = 0; i < BLINK_CYCLES; i++)
  {
    digitalWrite(LED, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(LED, LOW);
    delay(BLINK_DELAY);
  }
  ESP.deepSleep(SLEEP_TIME);
}

void loop() {}

#endif

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
