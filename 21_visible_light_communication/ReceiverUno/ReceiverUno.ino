#define LDR 2

void setup() {
  // put your setup code here, to run once:
  pinMode(LDR, INPUT);

  Serial.begin(9600);

}

void loop() {
  Serial.println(digitalRead(LDR));
  delay(200);

}
