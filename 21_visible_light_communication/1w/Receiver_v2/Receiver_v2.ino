#include <avr/sleep.h>
#include <VLC.h>

#define LED 0     // VLC will ignore pin 0 eg no transmission
#define LDR 2
#define DELAY_SCALE 2

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  Serial.begin(9600);

  // Set up callbacks to receive events
  vlc.setOnMessageReceived(onMessageReceived);  // When message completely received
  vlc.setOnMessageStarted(onMessageStarted);    // When message start detected
  vlc.setOnByteReceived(onByteReceived);        // When one byte of message received
  vlc.begin(DELAY_SCALE, LED, LDR);                       // Setup speed and TX and RX pins

  sleep_enable();
  sleep_cpu();
}

void goToSleep()
{
  Serial.println("Will sleep now");
  while ((UCSR0A & _BV (TXC0)) == 0) {} // wait for serial to finish
  sleep_enable();
  sleep_cpu();
}

// Called when message is completely received. Output and go to sleep
void onMessageReceived(const String& s) {
  Serial.println("\n-----------------------");
  Serial.print("Received message: '");
  Serial.print(s);
  Serial.println("'");

  goToSleep();
}

// Transmission started
void onMessageStarted() {
  Serial.println("* Transmission started\n-----------------------");
}

// Every time byte is received, this event will be fired
void onByteReceived(uint8_t b) {
  Serial.print((char)b);
}

void loop() {
  vlc.loop();
}
