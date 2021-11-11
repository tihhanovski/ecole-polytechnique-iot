#define LED 7
#define LDR 0
#define BTN 6
#define DELAY_SCALE 2

#include <VLC.h>

void setup() {
  pinMode(BTN, INPUT_PULLUP);
  Serial.begin(9600);
  vlc.begin(DELAY_SCALE, LED, LDR);     // Setup VLC. No callbacks this time, we only send
}

void loop() {
  if(digitalRead(BTN) == LOW)
  {
    Serial.println("click");
    unsigned long t = millis();
    // Sending is blocking
    String msg = "Hello, Arduino! \nI try to send a little bit longer message\n to see when receiver \nwill drift out of synchrony.";
    vlc.sendMessage(msg);

    t = millis() - t;
    Serial.print("\n------------------\nSent ");
    Serial.print(msg.length());
    Serial.print(" bytes in ");
    Serial.print(t);
    Serial.print("msec at speed ");
    Serial.print((double)(msg.length()) / t * 8000);
    Serial.println(" bps");

    delay(2000);
  }
  delay(200);
}
