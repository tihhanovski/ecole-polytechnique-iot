class VLCSender{
  public:

  uint8_t pinToWrite;

  uint8_t byteToSend;
  uint8_t bitToSend = 127;
  unsigned long bitDelay;
  unsigned long nextBitTime = 0;
  

  void begin(uint8_t pin, unsigned long msecPerByte) {
    pinToWrite = pin;
    pinMode(LED, pinToWrite);
    bitDelay = msecPerByte;
  }

  void sendPreamble()
  {
    Serial.println("Preamble started");
    unsigned long bitDelaySaved = bitDelay;
    bitDelay /= 2;
    sendSync(0b10101010);
    bitDelay = bitDelaySaved;
    Serial.println("\nPreamble finished");
  }

  void sendIdle()
  {
    Serial.println("\nSending idle");
    digitalWrite(pinToWrite, 0);
  }

  void printSync(String s) {

    sendPreamble();

    for(uint8_t c : s)
    {
      sendSync(c);
      Serial.println("");
    }
  
    sendIdle();

  }

  void sendSync(uint8_t b)
  {
    send(b);
    while(stillSending())
      loop();
  }

  void send(uint8_t b)
  {
    Serial.print("send('");
    Serial.print((char)b);
    Serial.print("'): ");
    byteToSend = b;
    bitToSend = 0b10000000; //first we send highest bit
  }
  
  inline bool stillSending()
  {
    return bitToSend;
  }
  
  void loop()
  {
    if(stillSending())
    {
      unsigned long t = millis();
      if(t >= nextBitTime)
      {
        nextBitTime = t + bitDelay;
        digitalWrite(pinToWrite, bitToSend & byteToSend);
        //Serial.print(t);
        Serial.print("\t");
        Serial.print((bitToSend & byteToSend) == 0 ? 0 : 1);
        bitToSend = bitToSend >> 1;
      }
    }
  }
  
};