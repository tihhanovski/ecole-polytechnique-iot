#define LED_BUILTIN 2
#define LED_PIN 5

#define BUTTON      4
#define ROTARY_SW   18
#define ROTARY_DT   19
#define ROTARY_CLK  21

int clk = 0;
int dt = 0;



void setup() {
  pinMode (LED_PIN, OUTPUT);
  pinMode (BUTTON, INPUT_PULLUP);

  pinMode(ROTARY_CLK, INPUT);
  pinMode(ROTARY_DT, INPUT);

  Serial.begin(115200);
  Serial.flush();
  Serial.println("Hello!");
}

int i = 0;

void loop() {

  int newCLK = digitalRead(ROTARY_CLK);
  int newDT = digitalRead(ROTARY_DT);
  if ( (newCLK != clk) || (newDT != dt) ) {
    Serial.print(i++);
    Serial.print(": ");
    Serial.print(newCLK);
    Serial.print(" ");
    Serial.println(newDT);
    clk = newCLK;
    dt = newDT;
  }

  /*
  Serial.print("read ");
  Serial.print(digitalRead(BUTTON));
  Serial.println("");
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  */
}
