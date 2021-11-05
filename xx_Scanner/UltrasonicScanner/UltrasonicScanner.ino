
/*
 Stepper Motor Control - one step at a time

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor will step one step at a time, very slowly.  You can use this to
 test that you've got the four wires of your stepper wired to the correct
 pins. If wired correctly, all steps should be in the same direction.

 Use this also to count the number of steps per revolution of your motor,
 if you don't know it.  Then plug that number into the oneRevolution
 example to see if you got it right.

 Created 30 Nov. 2009
 by Tom Igoe

 */

#define BTN_START 2
#define TRIG_PIN 4
#define ECHO_PIN 7

volatile bool stopRequested;

#include <Stepper.h>
#include <avr/sleep.h>

const unsigned long stepsPerRevolution = 2050;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper stpm(stepsPerRevolution, 8, 10, 9, 11);

int stepCount = 0;         // number of steps the motor has taken
int speed = 1;

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

void setup() {

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(13, 0);
  
  // initialize the serial port:
  Serial.begin(9600);

  stopRequested = false;

  attachInterrupt(digitalPinToInterrupt(BTN_START), startScan, FALLING);
}


void startScan()
{
  stopRequested = true;
  digitalWrite(13, 1);
  
}

void scan()
{
    stpm.setSpeed(10);
    stpm.step(-stepsPerRevolution / 2);

    int stepped = 0;
    stpm.setSpeed(speed);
    for(unsigned long i = 0; i <= 360; i += 5)
    {  
      if(stopRequested)
      {
        Serial.println("#####STOP#######");
        return;
      }
      int stepTo = (int)((double)(i * stepsPerRevolution) / 360);
      if(i == 360)
        stepTo = stepsPerRevolution;
      Serial.print("Step ");
      Serial.print(stepTo);
      stpm.step(stepTo - stepped);
      stepped = stepTo;
      Serial.print("\tsdegree: ");
      Serial.print(i);
      Serial.print("\tsteps: ");
      Serial.print(stepped);
      Serial.print("\t");
      //Serial.println(getDistance());
      Serial.println("");
      delay(50);
    }
    stpm.setSpeed(10);
    stpm.step(-stepsPerRevolution / 2);
}


void loop() {

  String cmd = "";
  while(Serial.available())
  {
    uint8_t incoming = Serial.read();
    if(incoming != 10)
      cmd = cmd + (char)incoming;
  }

  if(cmd.substring(0, 4) == "rot ")
  {
    int rot = cmd.substring(4).toInt();

    Serial.print("Rotate ");
    Serial.print(rot);
    Serial.print(" steps...");
    
    stpm.setSpeed(speed);
    stpm.step(rot);
    Serial.println(" done");
  }

  if(cmd.substring(0, 6) == "speed ")
  {
    speed = cmd.substring(6).toInt();
    Serial.print("New speed ");
    Serial.println(speed);
  }

  if(cmd == "scan")
  {
    Serial.println("Scan started");

    stopRequested = false;
    scan();

    digitalWrite(13, 0);
  }
  delay(50);

  // wait for transmit buffer to empty, see https://forum.arduino.cc/t/how-to-know-when-serial-has-finished-sending/116582/6
  //while ((UCSR0A & _BV (TXC0)) == 0) {}   

  // Go to sleep
  //sleep_enable();
  //sleep_cpu();
}
