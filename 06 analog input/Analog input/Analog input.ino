
//brightness thresholds
#define THR_DARK_DEFAULT 150    //dark threshold
#define THR_BRIGHT_DEFAULT 410  //light threshold

//pins for LEDs
#define PIN_RED 11
#define PIN_BLUE 12
#define PIN_GREEN 13

//#define DEBUG 1

int darkThreshold;
int brightThreshold;

void setup() {
  //Setup pins
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);

  darkThreshold = THR_DARK_DEFAULT;
  brightThreshold = THR_BRIGHT_DEFAULT;

  //Start debug output
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

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
  return analogRead(A0);
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
  else 
  {
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
  {
    //When pressing the TOP button, the current value of the light sensor will be recorded as threshold for the switch between RED and GREEN
    darkThreshold = brightness;
  }

  if (button == 'b')
  {
    //When pressing the BOTTOM button, the current value of the light sensor will be recorded as threshold for the switch between BLUE and GREEN.
    brightThreshold = brightness;
  }

    #ifdef DEBUG
    Serial.print("Current brightness ");
    Serial.print(brightness);
    Serial.print("; Dark threshold ");
    Serial.print(darkThreshold);
    Serial.print("; Bright threshold ");
    Serial.println(brightThreshold);
    delay(300);
    #endif


}