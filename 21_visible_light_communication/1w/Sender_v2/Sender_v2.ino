/* 
 * Visible light communication sender example without synchronization
 * Homework for INF471C - Connected Objects and the Internet of Things (IoT) at École polytechnique
 * Ilja Tihhanovski
 *
 * Inspired by 1-Wire protocol https://en.wikipedia.org/wiki/1-Wire
 * See more on work principles in VLC.h
 */

#define LED 7             // LED i.e. "transmitter" pin
#define LDR 0             // LDR i.e. "receiver" pin. Pin 0 will be ignored by the library
#define BTN 6             // Button pin. Pressing the button makes sender transmit the message
#define DELAY_SCALE 2     // Based on this value VLC library will calculate time intervals for different symbols.

#include <VLC.h>          // You may need to replace it to "VLC.h" if library is placed in the same folder with sketch
//#include "VLC.h"        // For the "local library placement"

void setup() {
  pinMode(BTN, INPUT_PULLUP);           // Setup button
  Serial.begin(9600);                   // Setup serial to output data
  vlc.begin(DELAY_SCALE, LED, LDR);     // Setup VLC. No callbacks this time, we only send
}

void loop() {
  if(digitalRead(BTN) == LOW)           // If button is pressed
  {
    Serial.println("click");
    unsigned long t = millis();         // Save the current time to calculate transmission speed later
    String msg = "We can use out very standard modules\nin the Arduino kit to transmit information through light.\nThat's the basis of the Li-Fi technology.";
    vlc.sendMessage(msg);               // Sending is blocking

    // Output time and speed
    t = millis() - t;
    Serial.print("\n------------------\nSent ");
    Serial.print(msg.length());
    Serial.print(" bytes in ");
    Serial.print(t);
    Serial.print("msec at speed ");
    Serial.print((double)(msg.length()) / t * 8000);
    Serial.println(" bps");

    delay(2000);                        // Primitive debouncing
  }
  delay(200);
}
