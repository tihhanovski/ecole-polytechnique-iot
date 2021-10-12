#define LED_R 3
#define LED_G 5
#define LED_B 6

//#define BUTTON      
#define ROTARY_SW   4
#define ROTARY_DT   7
#define ROTARY_CLK  8

#define BUTTON_R 9
#define BUTTON_G 10
#define BUTTON_B 11
#define BUTTON_N 12

void setup() {

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode (BUTTON_R, INPUT_PULLUP);
  pinMode (BUTTON_G, INPUT_PULLUP);
  pinMode (BUTTON_B, INPUT_PULLUP);
  pinMode (BUTTON_N, INPUT_PULLUP);

  pinMode(ROTARY_CLK, INPUT);
  pinMode(ROTARY_DT, INPUT);

  Serial.begin(9600);
  Serial.flush();
  Serial.println("Hello!");
}

int iCounter = 0;
bool justPressed = false;

byte red = 254;
byte green = 254;
byte blue = 254;

inline void noop()
{
  for (int i=0; i < 1600; i++) asm volatile ("NOP");
}

int clk = 0;
int dt = 0;

inline byte getDirection()
{
  byte ret = 0;
  int newCLK = digitalRead(ROTARY_CLK);
  int newDT = digitalRead(ROTARY_DT);
  noop();
  if ( (newCLK != clk) && newCLK == LOW ) { // CLK has changed from HIGH to LOW
    if ( dt == LOW)
      ret = -1;
    else
      ret = 1;
  }
  clk = newCLK;
  dt = newDT;
  return ret;
}

void loop() {

  byte dir = getDirection();

  if(dir != 0)
  {
    Serial.print("red: ");
    Serial.print(red);
    if(dir == 1)
      Serial.println(" + ");
    if(dir == -1)
      Serial.println(" - ");
  
    red = red + dir * 10;
    if(red < 1) red = 1;
    if(red > 254) red = 254;
  
  }

  analogWrite(LED_R, red + 1);
  analogWrite(LED_G, green + 1);
  analogWrite(LED_B, blue + 1);


  if(digitalRead(BUTTON_N) == 0)
  {
    if(!justPressed)
    {
      iCounter = 0;
      Serial.println("---------------------");
      justPressed = true;
    }
  }
  else
    justPressed = false;

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