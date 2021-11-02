#define LED 7
#define LDR 8
#define BUTTON 6

void setup() {
  pinMode(LED, OUTPUT);
  //pinMode(LDR, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  // put your setup code here, to run once:

}

void loop() {

  digitalWrite(LED, !digitalRead(BUTTON));

  //Serial.println(digitalRead(LDR));
  delay(100);
  
  // put your main code here, to run repeatedly:

}
