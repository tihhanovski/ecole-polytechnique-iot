void setup() {
  // put your setup code here, to run once:
   pinMode(5, OUTPUT); // 5 is D1

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(5, HIGH);
  delay(250);
  digitalWrite(5, LOW);
  delay(250);

}
