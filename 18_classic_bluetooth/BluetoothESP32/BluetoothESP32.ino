// Uses example code by Evandro Copercini - 2018
// https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/

#define LED 2
#define RATESTEP 100

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

bool ledOn = false;

void setup() {

  pinMode(LED, OUTPUT);
  digitalWrite(LED, ledOn);
  
  Serial.begin(115200);
  SerialBT.begin("ESP32test2"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}


int rate = 0;
unsigned long nt = 0;

void loop() {

  unsigned t;
  if(rate && (nt < (t = millis())))
  {
    ledOn = !ledOn;
    digitalWrite(LED, ledOn);
    nt = t + rate;
  }
  
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    int i = SerialBT.read();
    Serial.write(i);
    if(isDigit(i))
    {
      rate = RATESTEP * (i - '0');
      SerialBT.print("New blinking period is ");
      SerialBT.println(String(rate * 2));
    }
  }
  delay(20);
}
