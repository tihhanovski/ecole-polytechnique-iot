class VLCPhysical {
  public:
    uint8_t pinToWrite;
    unsigned long bitDelay;

  void setDelay(uint32_t iDelay) {
    bitDelay = iDelay;
  }

  VLCPhysical(uint8_t pin, uint32_t iDelay) {
    pinToWrite = pin;
    bitDelay = iDelay;
  }

  void send(bool b) {
    digitalWrite(pinToWrite, b);
  }
};