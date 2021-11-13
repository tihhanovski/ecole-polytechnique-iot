#define LED 7
#define BTN 6

//#include <VLC.h>

#define DELAY_SCALE 3
#define DELAY_START DELAY_SCALE * 10
#define DELAY_FINISH DELAY_SCALE * 15
#define DELAY_1 DELAY_SCALE
#define DELAY_0 DELAY_SCALE * 3
#define DELAY_PAUSE DELAY_SCALE * 3

class Sender {

  uint8_t portToWrite;

  public:

    void begin(uint8_t port)
    {
      portToWrite = port;
      pinMode(port, OUTPUT);
    }

    inline void impuls(int iDelay) {
      digitalWrite(portToWrite, 1);
      delay(iDelay);
      digitalWrite(portToWrite, 0);
      delay(DELAY_PAUSE);
    }

    void sendStart() {
      impuls(DELAY_START);
    }

    inline void sendFinish() {
      impuls (DELAY_FINISH);
    }

    void sendBit(bool b) {
      impuls (b ? DELAY_1 : DELAY_0);
    }

    void print(String s)
    {
      unsigned long t = millis();
      sendStart();
      for(uint8_t c : s)
      {
        Serial.print((char)c);
        Serial.print("\t");
        Serial.print(c, BIN);
        Serial.print("\t");
        for(uint8_t i = 128; i > 0; i = i >> 1)
        {
          Serial.print(c & i ? 1 : 0);
          sendBit(c & i);
        }
        Serial.println("");
      }
      sendFinish();
      t = millis() - t;
      Serial.print("Sent ");
      Serial.print(s.length());
      Serial.print(" bytes in ");
      Serial.print(t);
      Serial.print("msec at speed ");
      Serial.print((double)(s.length()) * 8000 / t);
      Serial.println(" bps");
    }

};

Sender sender;

void setup() {
  // put your setup code here, to run once:
  pinMode(BTN, INPUT_PULLUP);
  Serial.begin(9600);
  sender.begin(LED);

}

void loop() {

  if(digitalRead(BTN) == LOW)
  {
    Serial.println("click");
    sender.print("Hello, Arduino! \nI try to send a little bit longer message\n to see when receiver \nwill drift out of synchrony.");
    delay(2000);
  }
  delay(200);
}
