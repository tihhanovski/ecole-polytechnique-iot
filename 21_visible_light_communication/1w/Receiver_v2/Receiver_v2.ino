/* 
 * Visible light communication receiver example without synchronization
 * Homework for INF471C - Connected Objects and the Internet of Things (IoT) at École polytechnique
 * Ilja Tihhanovski
 *
 * Inspired by 1-Wire protocol https://en.wikipedia.org/wiki/1-Wire
 * See more on work principles in VLC.h
 */

#include <avr/sleep.h>    // Receiver will sleep between transmissions and begin of transmission will wake it up
#include <VLC.h>          // You may need to replace it to "VLC.h" if library is placed in the same folder with sketch
//#include "VLC.h"        // For the "local library placement"

#define LED 0             // VLC will ignore pin 0 i.e. no transmission
#define LDR 2             // Current version of library works only with pin 2, but it is possible to upgrade the library to use any pin with interrupt ability
#define DELAY_SCALE 2     // Based on this value VLC library will calculate time intervals for different symbols.

unsigned long t;          // Begin of transmission to calculate speed

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);          // Setup sleep mode

  Serial.begin(9600);

  // Set up callbacks to receive events
  vlc.setOnMessageReceived(onMessageReceived);  // When message completely received
  vlc.setOnMessageStarted(onMessageStarted);    // When message start detected
  vlc.setOnByteReceived(onByteReceived);        // When one byte of message received
  vlc.begin(DELAY_SCALE, LED, LDR);             // Setup speed and TX and RX pins

  goToSleep();                                  // Put controller to sleep
}

void loop() {
  vlc.loop();                                   // We should call it to let library process the data arrived
}

// Wait for serial communication to finish and put controller to sleep
void goToSleep()
{
  Serial.println("Will sleep now");
  while ((UCSR0A & _BV (TXC0)) == 0) {}         // wait for serial to finish
  sleep_enable();
  sleep_cpu();
}

// Called when message is completely received. Output and go to sleep
void onMessageReceived(const String& s) {
  t = millis() - t;
  Serial.println("\n-----------------------");
  Serial.print("Received message: '");
  Serial.print(s);
  Serial.print("'\nTime: ");
  Serial.print(t);
  Serial.print(" msec, speed ");
  Serial.print((double)(s.length()) / t * 8000);
  Serial.println(" bps.");

  goToSleep();
}

// Transmission started
void onMessageStarted() {
  t = millis();
  Serial.println("* Transmission started\n-----------------------");
}

// Every time byte is received, this event will be fired
void onByteReceived(uint8_t b) {
  Serial.print((char)b);
}

