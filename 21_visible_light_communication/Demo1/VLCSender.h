
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

  void printSync(String s) {
    for(uint8_t c : s)
    {
      send(c);
      while(bitToSend)
        loop();
    }
  }

  void send(uint8_t byte)
  {
    byteToSend = byte;
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
        Serial.print(t);
        Serial.print("\t");
        Serial.println((bitToSend & byteToSend) == 0 ? 0 : 1);
        bitToSend = bitToSend >> 1;
      }
    }
  }
  
};
