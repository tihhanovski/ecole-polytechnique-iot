#define MAXSCALE 4
#define BUTTON 2
#define LED 9
#define SCALE_LOWEST 15624    // Lowest rate : 1 Hz
#define SCALE_HIGHEST 3906    // Hightest rate : 4 Hz

volatile unsigned long counter = 0;
volatile unsigned long counterTotal = 0;
volatile unsigned int scale = SCALE_HIGHEST;

// Code to be executed on button interrupt
void switchTimer()
{
  scale = scale >> 1;         // modify the scale
  if(scale < SCALE_HIGHEST)   // if rate is too high
    scale = SCALE_LOWEST;     // then set scale to default

  OCR1A = scale;              // scale the register
  TCNT1 = 0;                  // set internal timer counter 1 to zero
  counter = 0;                // reset our counter
}


void setup() {
  noInterrupts();             //stop interrupts while configuring
  TCCR1A = 0;                 // reset timer1 setup registers
  TCCR1B = 0; 

  switchTimer();              // part of the setup code is there just to reuse code
  TCCR1B |= (1 << WGM12);     // enable CTC mode

  // set scale to 1 / 1024
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);

  TIMSK1 |= (1 << OCIE1A);    // setup interrupt on timer compare

  interrupts();               //allow interrupts again

  // Setup button interrupt
  attachInterrupt(digitalPinToInterrupt(BUTTON), switchTimer, FALLING );

  // Pin modes
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  // Start serial
  Serial.begin(9600);
  while(!Serial){}
}

// Timer interrupt
// Just increase counters and blink the led
ISR(TIMER1_COMPA_vect)
{
  counter++;
  counterTotal++;
  digitalWrite(9, !digitalRead(9));
}

void loop() {
  // Output data with some frequency (about once every second)
  Serial.print("scale: ");
  Serial.print(scale);
  Serial.print("\tcounter: ");
  Serial.print(counter);
  Serial.print("\ttotal: ");
  Serial.println(counterTotal);
  delay(1000);
}