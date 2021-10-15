#define BTN1 2
#define BTN2 4

/*
 * R - G
 * YR - Y
 * G - R
 * Y - YR
 */

void setup() {
  // put your setup code here, to run once:

  pinMode(BTN1, OUTPUT);
  pinMode(BTN2, OUTPUT);
  digitalWrite(BTN1, HIGH);
  digitalWrite(BTN2, HIGH);

}

bool initialized = false;

void loop() {

  if(!initialized)
  {
    for(byte i = 0; i < 2; i++)
    {    
      digitalWrite(BTN2, LOW);
      delay(100);
      digitalWrite(BTN2, HIGH);
      delay(100);
    }
    initialized = true;
  }
  
  while(true)
  {
    digitalWrite(BTN1, HIGH);
    digitalWrite(BTN2, HIGH);
    delay(1000);
    digitalWrite(BTN1, LOW);
    digitalWrite(BTN2, LOW);
    delay(100);
  }

}
