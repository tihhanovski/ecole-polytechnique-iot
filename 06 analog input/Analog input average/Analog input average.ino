//brightness thresholds
#define THR_DARK_DEFAULT 150    //dark threshold
#define THR_BRIGHT_DEFAULT 410  //light threshold

//pins for LEDs
#define PIN_RED 11
#define PIN_BLUE 12
#define PIN_GREEN 13
#define AVG_COUNT 10


int darkThreshold;
int brightThreshold;
int history[AVG_COUNT];

void setup() {
  //Setup pins
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);

  darkThreshold = THR_DARK_DEFAULT;
  brightThreshold = THR_BRIGHT_DEFAULT;

  //initialize history
  for(auto i = 0; i < AVG_COUNT; history[i++] = 0);
}

char getButtonPressed()
{
  int bv = analogRead(A1);
  if(bv < 100)
    return 't';
  if(bv > 450 && bv < 550)
    return 'b';
  return ' ';   //assume that nothing is pressed
}

int getBrightness()
{
  //shift history
  long int sum = 0;
  for(auto i = 1; i < AVG_COUNT; i++)
  {
    history[i - 1] = history[i];
    sum += history[i];
  }
  int s = analogRead(A0);
  history[AVG_COUNT - 1] = s;
  sum += s;

  return sum / AVG_COUNT;
}

void loop(){
  //Initialize output values for all three leds to zero
  byte r = LOW;
  byte g = LOW;
  byte b = LOW;

  //Get "brigtness value" from analog input
  int brightness = getBrightness();

  //Compare and set one of led values to HIGH
  if(brightness > brightThreshold)
    r = HIGH;
  else {
    if(brightness < darkThreshold)
      b = HIGH;
    else
      g = HIGH;
  }

  //Switch the leds
  digitalWrite(PIN_RED, r);
  digitalWrite(PIN_GREEN, g);
  digitalWrite(PIN_BLUE, b);

  char button = getButtonPressed();
  if (button == 't')
    darkThreshold = brightness;

  if (button == 'b')
    brightThreshold = brightness;
}