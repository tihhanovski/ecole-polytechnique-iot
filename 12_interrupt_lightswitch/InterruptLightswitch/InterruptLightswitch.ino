/*

    

*/


#define BUTTON_ON 2
#define BUTTON_OFF 3
#define LED 13

#include <avr/sleep.h>

volatile bool lightsOn;

void setup() {
  //Sleep mode                              LED on (mA)     LED off (mA)
  //SLEEP_MODE_PWR_DOWN                     33.3            24.2
  //SLEEP_MODE_STANDBY                      33.7            24.5
  //SLEEP_MODE_EXT_STANDBY                  34.2            25.1
  //SLEEP_MODE_PWR_SAVE                     34.3            25.1
  //SLEEP_MODE_ADC                          38.4            29.2
  //SLEEP_MODE_IDLE - not sleeping at all   46.4            36.8
  // Currents measured with UNI-T UT132C multimeter
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
