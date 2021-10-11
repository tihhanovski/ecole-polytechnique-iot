/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - Button's Up...
 * Ilja Tihhanovski
 * 
 * when switch A is pushed, the LED will turn on. 
 * when switch B is pushed, the LED will turn off. 
 * 
 */

//Define pins for buttons and LED
#define BUTTON_A 9
#define BUTTON_B 8
#define MY_LED 5

void setup() 
{
  //Setup buttons:
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);

  //Setup LED
  pinMode(MY_LED, OUTPUT);

  //Initialize LED (switch off);
  digitalWrite(MY_LED, LOW);
}

void loop() 
{
  if (digitalRead(BUTTON_A) == LOW)   //when switch A is pushed
    digitalWrite(MY_LED, HIGH);       //the LED will turn on. 
    
  if (digitalRead(BUTTON_B) == LOW)   //when switch B is pushed
    digitalWrite(MY_LED, LOW);        //the LED will turn off.
}
