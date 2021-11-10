/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * I've Got A Light Case of the Blue(tooth)s
 * Ilja Tihhanovski
 * 
 * Uses example code by Evandro Copercini - 2018
 * https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/
 * Uses examples from ESP32 BluetoothSerial library by Richard Li - 2020
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial/examples
 *
 */


#define LED 2                           // Built-in LED
#define RATESTEP 100                    // so for 5 LED will be 500 msec on and 500 msec off
#define BT_DEVICE_NAME "ESP32test2"     // our device name in BT devices list

#include "BluetoothSerial.h"

// Check if Bluetooth enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;           // Instantiate Serial connection to BT

bool ledOn = false;                 // true if LED is on
bool clientConnected = false;       // true if we detected connected client
bool confirmRequestPending = false; // true if there is pairing request
int rate = 0;                       // Current blinking rate 
unsigned long timeToBlink = 0;      // Next blinking time (time to switch led on or off)

void setup() {

  // By default we assume that there is nobody connected and LED is off
  ledOn = false;
  clientConnected = false;

  pinMode(LED, OUTPUT);             // setup pin (built in LED on ESP32 dev module)
  digitalWrite(LED, ledOn);
  
  Serial.begin(115200);             // Start serial
  while(!Serial) {}                 // and wait until it initializes

  // Enable SSP (Secure Simple Pairing)
  SerialBT.enableSSP();

  // Connect callbacks to be able to react on pairing request
  SerialBT.onConfirmRequest(BTConfirmRequestCallback);
  SerialBT.onAuthComplete(BTAuthCompleteCallback);

  // We could process data from bluetooth using next callback
  // But I discovered, that if I use this method, then
  // SerialBT.available() will always return false in loop()
  //SerialBT.onData(BT_onData);

  // Start BT and setup name
  SerialBT.begin(BT_DEVICE_NAME);
  Serial.print("\nDevice '");
  Serial.print(BT_DEVICE_NAME);
  Serial.println("' is started, now you can pair it with bluetooth!");

}

void BTConfirmRequestCallback(uint32_t numVal) {
  confirmRequestPending = true;
  Serial.print("ConfirmRequestCallback called with argument ");
  Serial.println(numVal);
}

void BTAuthCompleteCallback(boolean success) {
  confirmRequestPending = false;
  if (success)
    Serial.println("Pairing success");
  else
    Serial.println("Pairing failed, rejected by user");
}

void BT_onData(const uint8_t *buffer, size_t size)
{
  Serial.print("* has data: '");
  for(size_t i = 0; i < size; i++)
    Serial.print(buffer[i]);
  Serial.println("'");
}

void loop() {

  // React to pairing request
  if (confirmRequestPending) {
    SerialBT.confirmReply(true);        // Here we always confirm incoming pairing request
    //SerialBT.confirmReply(false);     // But can always decline for example
  }

  // Figure out if another device is connected. Output a message if "connection status" is changed
  if(!clientConnected && SerialBT.hasClient()) {
    clientConnected = true;
    Serial.println("Client connected");
  }

  if(clientConnected && !SerialBT.hasClient()) {
    clientConnected = false;
    Serial.println("Client disconnected");
  }

  // Blinking
  unsigned long t;
  if(rate && (timeToBlink < (t = millis())))  // If blinking is on (rate != 0) and time to switch (nt) came
  {
    ledOn = !ledOn;                           // Flip the led
    digitalWrite(LED, ledOn);
    timeToBlink = t + rate;                   // Calculate next time to flip
  }

  // Output data from Serial (USB) to bluetooth
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  // Handle data coming from BT
  if (SerialBT.available()) {                       // If something is available
    int i = SerialBT.read();                        // We reed it byte at a time
    //SerialBT.print("Character from Bluetooth: '");  // Output character back to BT
    SerialBT.print((char)i);
    //SerialBT.println("': ");
    Serial.write(i);                                // Output character to Serial
    if(isDigit(i))                                  // If character corresponds to digit
      rate = RATESTEP * (i - '0');                  // Calculate a new blinking rate
      if(rate == 0)
        SerialBT.println("Stop blinking");
      else {
        SerialBT.print("New blinking period is ");
        SerialBT.println(String(rate * 2));
      }
    }
    else
      SerialBT.println("Ignoring");
  }
  delay(20);                                        // Wait a little
}
