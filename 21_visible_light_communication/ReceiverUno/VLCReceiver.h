class VLCReceiver {
public:

  uint8_t pinToRead;

  unsigned long bitDelay;
  unsigned long sampleDelay;
  unsigned long nextBitTime = 0;
  unsigned long nextSampleTime = 0;
  int samplingRate = 1;

  bool byteReady = false;
  bool enabled = false;

  unsigned long sample;
  int highBorder;

  uint8_t currentByte;

  uint8_t bitNo = 0;

  void begin(uint8_t pin, unsigned long msecPerBit, int iSamplingRate, int high)
  {
    pinToRead = pin;
    bitDelay = msecPerBit;
    samplingRate = iSamplingRate;
    sampleDelay = bitDelay / iSamplingRate;
    
    highBorder = high;
    enabled = true;
  }

  void stop()
  {
    enabled = false;
  }

  uint8_t read()
  {
    return currentByte;
    byteReady = false;
    bitNo = 0;
  }

  void pushToBuffer()
  {
    Serial.print("Byte received: ");
    Serial.println(currentByte);
  }

  void loop()
  {
    if(!enabled)
      return;
    unsigned long t = millis();
    if(t >= nextSampleTime)
    {
      nextSampleTime = t + sampleDelay;
      int reading = digitalRead(pinToRead);
      Serial.print(t);
      Serial.print(" -> reading: ");
      Serial.println(reading);
      sample += reading;
    }

    if(t >= nextBitTime)
    {
      nextBitTime = t + bitDelay;
      //uint8_t cb = (sample / samplingRate) > highBorder ? 1 : 0;
      uint8_t cb = (sample > (samplingRate / 2));

      Serial.print(t);
      Serial.print(" -> BIT: sample: ");
      Serial.print(sample);
      Serial.print("\tbit = ");
      Serial.println(cb);

      sample = 0;
      currentByte = currentByte << 1;
      currentByte |= cb;
      bitNo++;

      if(bitNo > 8)
      {
        byteReady = true;
        //TODO put byte to buffer, get ready for next byte
        pushToBuffer();
        bitNo = 0;
      }
    }
  }
  
};
