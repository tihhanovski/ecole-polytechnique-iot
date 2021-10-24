#define MAXSCALE 4
#define BUTTON 2
#define LED 9

volatile unsigned long counter = 0;
volatile unsigned long counterTotal = 0;
volatile uint8_t scale = 4;

void switchTimer()
{
  scale = scale << 1;
  if(scale > 4)
    scale = 1;

  OCR1A = 15624 / scale;      // установка регистра совпадения
  TCNT1 = 0;
  counter = 0;
}


void setup() {
  // put your setup code here, to run once:
  noInterrupts(); //stop interrupts while configuring

  TCCR1A = 0; // установить регистры в 0
  TCCR1B = 0; 

  switchTimer();  
  TCCR1B |= (1 << WGM12); // включение в CTC режим

  // Установка битов CS10 и CS12 на коэффициент деления 1024
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);

  TIMSK1 |= (1 << OCIE1A);  // включение прерываний по совпадению

  interrupts(); //allow interrupts again

  attachInterrupt(digitalPinToInterrupt(BUTTON), switchTimer, FALLING );

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);

  Serial.begin(9600);
  while(!Serial){}
}

ISR(TIMER1_COMPA_vect)
{
  counter++;
  counterTotal++;
  digitalWrite(9, !digitalRead(9));
}

void loop() {
  Serial.print("scale: ");
  Serial.print(scale);
  Serial.print("\tcounter: ");
  Serial.print(counter);
  Serial.print("\ttotal: ");
  Serial.println(counterTotal);
  delay(1000);
}