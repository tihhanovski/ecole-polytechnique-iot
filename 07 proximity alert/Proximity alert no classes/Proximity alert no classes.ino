/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - Proximity alert
 * Ilja Tihhanovski
 *
 * uses https://create.arduino.cc/projecthub/SURYATEJA/use-a-buzzer-module-piezo-speaker-using-arduino-uno-89df45
 * as an example of buzzer usage
 * 
 * 
 */

// ultrasonic sensor pins
#define TRIG_PIN 12
#define ECHO_PIN 11

#define BUTTON_PIN 10                 // shutdown button
#define BUZZER_PIN 8                  // buzzer

#define BUZZ_DURATION 800             // Actually it is half of buzz duration in milliseconds
#define DISTANCE_ALERTED_MAX 40       // Distances over this are considered as far
#define DISTANCE_ALERTED_CRITICAL 10  // On distances under this value we should hear buzzer
#define LEDS_COUNT 5                  // Count of LEDs
#define INITIAL_BUZZER_FREQ 700       // Initial buzzer frequency

// Debug stuff. Uncomment #define DEBUG 1 to use serial debug
//#define DEBUG 1
#define DEBUG_DELAY 500               // delay between debug messages in milliseconds

// Melody buzzer functionality encapsulated
//class Buzzer {
  unsigned long timeOff;              // time when buzzer should stop
  unsigned int freq;                  // initial frequency
  uint8_t pin;                        // buzzer pin

//public:
  void buzzer_init(uint8_t iPin) {             // initialize buzzer variables
    pin = iPin;
    buzzer_stop();
  }

  bool buzzer_buzzing() {                     // returns true if buzzer is on
    return timeOff > 0;
  }

  void buzzer_run() {                        // should be called from loop on every iteration
    long timeLeft = timeOff - millis();
    if(!timeOff || timeLeft < 0)
      buzzer_stop();
    else
    {
      if(timeLeft > BUZZ_DURATION)
        tone(pin, freq + BUZZ_DURATION * 2 - timeLeft);   // frequency goes up in linear fashion during first part of melody
      else
        noTone(pin);    // no sound during second half of melody
    }
  }

  void buzzer_buzz(unsigned int iFreq) {    // starts buzzer
    if(buzzer_buzzing())
      return;
    freq = iFreq;              // remember initial frequency
    if(freq > 0)
      timeOff = millis() + BUZZ_DURATION * 2; // remember time to finish melody
    else
      buzzer_stop();                                 // if frequency given is wrong, do not start
  }

  void buzzer_stop() {                      // stop buzzing
    timeOff = 0;
    noTone(pin);
  }


//Buzzer buzzer;                    // buzzer instance

uint8_t leds[] = {9, 7, 6, 5, 4}; // LEDs' pins
bool shutDownAlert;               // Should we shut the sound alert down?

#ifdef DEBUG
  unsigned long debugTime;
#endif

void setup() {

  // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  for(auto i = 0; i < LEDS_COUNT; pinMode(leds[i++], OUTPUT));
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize buzzer (set the pin)
  buzzer_init(BUZZER_PIN);

  // By default sound alert is on
  shutDownAlert = false;

  #ifdef DEBUG
    Serial.begin(9600);
    Serial.flush();
  #endif
}

// Returns distance (mostly copy from Moodle lesson)
// https://moodle.polytechnique.fr/mod/lesson/view.php?id=180139&pageid=3595&startlastseen=no
double getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(1);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long duration = pulseInLong(ECHO_PIN, HIGH);
  return (duration / 2) / 29.1;
}

void loop() {
  if(digitalRead(BUTTON_PIN) == LOW)  // Check if shutdown button is pressed and shut down the alarm
    if(buzzer_buzzing())              // but only if buzzer is working
      shutDownAlert = true;

  double distance = getDistance();    // Get the distance
  uint8_t pinIndex = LEDS_COUNT;      // Last index of LED to be switch on
  // For example if pinIndex = 0, then only first LED will be on
  // If pinIndex = 3, then first four LEDs will be on

  // If the distance is less than max alertable, then calculate index, else just use initial maximum value
  if(distance < DISTANCE_ALERTED_MAX)
  {
    pinIndex = distance >= DISTANCE_ALERTED_CRITICAL ?
      map(distance, DISTANCE_ALERTED_CRITICAL, DISTANCE_ALERTED_MAX, 0, LEDS_COUNT) :
      0;
  }

  // Alert switch off button will automatically reset when distance is larger than critical (or max alerted)
  if(distance > DISTANCE_ALERTED_CRITICAL)
    shutDownAlert = false;    

  // Switch the LEDs on and off
  for(auto i = 0; i < LEDS_COUNT; i++)
    digitalWrite(leds[i], i > pinIndex ? LOW : HIGH);

  if(distance < DISTANCE_ALERTED_CRITICAL)  // When distance is below critical
    if(shutDownAlert)                       // and buzzer shutdown button is not pressed
      buzzer_stop();
    else
      buzzer_buzz(INITIAL_BUZZER_FREQ);     // make the noize

  buzzer_run();                             // process current buzzer state on every loop iteration

  #ifdef DEBUG
    if(millis() > debugTime)
    {
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.print("; pinIndex: ");
      Serial.print(pinIndex);
      Serial.println("");
      debugTime = millis() + DEBUG_DELAY;
    }
  #endif
}
