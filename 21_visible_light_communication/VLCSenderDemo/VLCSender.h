/* 
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Visible light communication with synchronization
 * Ilja Tihhanovski
 *
 * Sender library
 */

class VLCSender{

  uint8_t pinToWrite;
  uint8_t byteToSend;
  uint8_t bitToSend = 127;
  unsigned long bitDelay;
  unsigned long packetSize;
  unsigned long nextBitTime = 0;
  
  public:

  // Sender Initialization
  void begin(uint8_t pin, unsigned long msecPerByte, unsigned int packetSize) {
    pinToWrite = pin;
    this->packetSize = packetSize;
    bitDelay = msecPerByte;
    pinMode(LED, pinToWrite);
  }

  // Preamble is one byte 10101010 sent with double speed
  void sendPreamble()
  {
    Serial.println("Preamble started");
    unsigned long bitDelaySaved = bitDelay; // Save normal speed
    bitDelay /= 2;                          // Make speed 2x higher
    sendSync(0b10101010);                   // Transmit 10101010
    bitDelay = bitDelaySaved;               // Recover old normal speed
    Serial.println("\nPreamble finished");
  }

  // After each packet we must send IDLE signal.
  // Idle is just long enough LOW period.
  // Empirically I found that three bytes of 0 is long enough.
  void sendIdle()
  {
    while(micros() < nextBitTime)       // Output last bit of the last byte
      delayMicroseconds(50);            // We cant just do delayMicroseconds(200000), it will not work
    Serial.println("\nSending idle");
    sendSync(0);
    sendSync(0);
    sendSync(0);
  }

  // Send the message
  void printSync(String s) {

    for(size_t i = 0; i < s.length(); i++)    // For every byte in message
    {
      if(i % packetSize == 0)                 // If packet is over
      {
        if(i > 0)                             // Output IDLE after every packet
          sendIdle();
        sendPreamble();                       // Sent the PREAMBLE before every packet
      }
      
      sendSync(s[i]);                         // Output the byte
      Serial.println("");

    }
    sendIdle();                               // Output IDLE after last packet
  }

  // Output one byte synchronously
  void sendSync(uint8_t b)
  {
    send(b);
    while(stillSending())
      loop();
  }

  // Start asynchronous sending of one byte
  void send(uint8_t b)
  {
    Serial.print("send '");
    Serial.print((char)b);
    Serial.print("' = ");
    Serial.print(b, BIN);
    Serial.print(" ");
    byteToSend = b;
    bitToSend = 0b10000000; //first we send highest bit
  }
  
  // True if there are still bits to send in current byte
  inline bool stillSending() {
    return bitToSend;
  }
  
  // Bit sending loop
  void loop() {
    if(stillSending())
    {
      unsigned long t = micros();
      if(t >= nextBitTime) {
        nextBitTime = t + bitDelay;
        digitalWrite(pinToWrite, bitToSend & byteToSend);     // Output current bit of byte we send
        Serial.print("");
        Serial.print(bitToSend & byteToSend ? 1 : 0);
        bitToSend = bitToSend >> 1;                           // Move to the next bit
      }
    }
  }
  
};