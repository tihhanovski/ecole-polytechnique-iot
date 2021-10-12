#define LED_R 5
#define LED_G 
#define LED_B

#define CHANNEL_R 0
#define CHANNEL_G 1
#define CHANNEL_B 2

#define BUTTON      4
#define ROTARY_SW   18
#define ROTARY_DT   19
#define ROTARY_CLK  21


void setup() {
  //pinMode (LED_R, ANALOG);

  //setup PWM -- ESP32
  ledcAttachPin(LED_R, CHANNEL_R);
  //ledcAttachPin(LED_G, CHANNEL_G);
  //ledcAttachPin(LED_B, CHANNEL_B);

  //ledcSetup(CHANNEL_R, 1000, 8);
  //ledcSetup(CHANNEL_G, 1000, 8);
  //ledcSetup(CHANNEL_B, 1000, 8);
  
  pinMode (BUTTON, INPUT_PULLUP);

  pinMode(ROTARY_CLK, INPUT);
  pinMode(ROTARY_DT, INPUT);

  Serial.begin(115200);
  Serial.flush();
  Serial.println("Hello!");
}

int iCounter = 0;
bool justPressed = false;

byte red = 255;

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
  
    ledcWrite(0, red + 1);
  }


  if(digitalRead(BUTTON) == 0)
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
