/* 
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Visible light communication with synchronization
 * Ilja Tihhanovski
 *
 * Sender sketch
 */

#define LED 7
#define BTN 6

#include "VLCSender.h"

#define SENDER_PERIOD 5000  //In microseconds
#define PACKET_SIZE 5       //Packet size (bytes). With packet longer than 10 bytes will drift out of synchrony

VLCSender sender;

void setup() {

  // Setup pin, start serial
  pinMode(BTN, INPUT_PULLUP);
  Serial.begin(9600);
  while(!Serial);

  // Initialize sender with LED, bit duration in usec and packet size
  sender.begin(LED, SENDER_PERIOD, PACKET_SIZE);

  Serial.println("Press the button to send");
}

bool pressed;

void loop() {

  if(digitalRead(BTN) == LOW)
  {
    // If button is pressed, transmit
    sender.printSync("Hello, Arduino!!!");
    delay(2000);
  }
  delay(200);
}
