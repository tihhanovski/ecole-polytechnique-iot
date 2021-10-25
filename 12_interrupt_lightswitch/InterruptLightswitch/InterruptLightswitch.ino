#define BUTTON_ON 2
#define BUTTON_OFF 3
#define LED 13

#include <avr/sleep.h>

volatile bool lightsOn;

void setup() {
  //SLEEP_MODE_PWR_DOWN
  //SLEEP_MODE_STANDBY
  //SLEEP_MODE_EXT_STANDBY
  //SLEEP_MODE_PWR_SAVE
  //SLEEP_MODE_ADC
  //SLEEP_MODE_IDLE - not sleeping at all
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  pinMode(BUTTON_ON, INPUT_PULLUP);
  pinMode(BUTTON_OFF, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  lightsOn = false;
  digitalWrite(LED, LOW);


  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(BUTTON_ON), lightOn, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_OFF), lightOff, FALLING);

  //LOW - lots of awakenings
  //RISING - switch when button being released
  //FALLING - switch when button being released
  //CHANGE - twice per press

}

void lightOn()
{
  if(!lightsOn)
    digitalWrite(LED, lightsOn = true);
}

void lightOff()
{
  if(lightsOn)
    digitalWrite(LED, lightsOn = false);
}

void loop() {
  Serial.println("Going to sleep...");
  delay(100);
  sleep_enable();
  sleep_cpu();
}
