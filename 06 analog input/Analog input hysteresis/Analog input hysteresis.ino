/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - Analog input
 * Ilja Tihhanovski
 *  
 * Handling analog input using hysteresis to avoid reaction to signal bouncing around threshold.
 */


// Brightness thresholds defaults
#define THR_DARK_DEFAULT 150    // Dark threshold
#define THR_BRIGHT_DEFAULT 410  // Light threshold
#define THR_BORDER 20           // Threshold border for hysteresis

// Pins for LEDs
#define PIN_RED 11
#define PIN_BLUE 12
#define PIN_GREEN 13

#define PIN_PHOTORESISTOR A0
#define PIN_BUTTONS A1


// Uncomment to enable debug output to Serial
//#define DEBUG 1

// Brightness and darkness thresholds
int darkThreshold;
int brightThreshold;

// State memory for dark and bright state
bool isDark = false;
bool isBright = false;


void setup() {
  // Setup pins
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);

  // Initialize state thresholds
  darkThreshold = THR_DARK_DEFAULT;
  brightThreshold = THR_BRIGHT_DEFAULT;

  // Start debug output
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
}

// Check if and which button is pressed
// returns 't', 'b' for top and bottom button or ' ' if nothing is pressed
char getButtonPressed() {
  int bv = analogRead(PIN_BUTTONS);
  if(bv < 100)
    return 't';
  if(bv > 450 && bv < 550)
    return 'b';
  return ' ';   //assume that nothing is pressed
}

// Method for checking state
// Optimized to being completely impossible to understand
// Arguments:
//  state - reference to state checked
//  brightness - current brightness
//  stateThreshold - threshold compared to brightness
//  thresholdBorder - hysteresis addition to threshold (it is harder to leave the state than to enter the state)
//  comparationSign - '>' or '<'
void checkState(bool &state, int brightness, int stateThreshold, int thresholdBorder, char comparationSign) {
  int sign = (comparationSign == '>' ? -1 : 1);
  state = sign * brightness < (sign * stateThreshold + (state ? thresholdBorder : 0));
}

void loop(){
  // Initialize output values for all three leds to zero
  byte r = LOW;
  byte g = LOW;
  byte b = LOW;

  // Get "brigtness value" from analog input
  int brightness = analogRead(PIN_PHOTORESISTOR);

  // Check bright state
  checkState(isBright, brightness, brightThreshold, THR_BORDER, '>');
  if (isBright)
    r = HIGH;
  else
  {
    checkState(isDark, brightness, darkThreshold, THR_BORDER, '<'); // Bright state has priority over dark, so we check dark state only if we are sure it is not bright
    if (isDark)
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
    #ifdef DEBUG
      Serial.print("t pressed ");
    #endif
  }

  if (button == 'b')
  {
    //When pressing the BOTTOM button, the current value of the light sensor will be recorded as threshold for the switch between BLUE and GREEN.
    brightThreshold = brightness;
    #ifdef DEBUG
      Serial.print("b pressed ");
    #endif
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