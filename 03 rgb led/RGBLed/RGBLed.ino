/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * Arduino basics - LED Disco Light - Pulse-Width Modulation
 * Ilja Tihhanovski
 * 
 * Values of red, green and blue component will be defined by three sinusoids
 * green shifted by 2/3 pi to the right compared to red
 * blue shifted by 2/3 pi to the right compared to green
 * Also sinusoids will be amplified (see AMPLITUDE) and shifted up so that result will be between 0 and 2* AMPLITUDE
 */
 
// Define Pins
#define RED 3
#define GREEN 5
#define BLUE 6
#define DELAYTIME 50 // How long will Arduino wait after each change
#define DELTAX 0.314 // On each iteration we will increase x for sinusoids by this value
#define AMPLITUDE 64 // Amplitude of our sinusoids. Maximum value is 127, but it was too bright for me.

double x = 0;        // sinusoids argument

void setup() {
 Serial.begin(9600);          // We use Serial to output debug information
 delay(200);                  // I tried to use this delay to give Serial some time to initialize. Don't know actually does it work.
 Serial.println("Hello RGB");
 pinMode(RED, OUTPUT);        // Setup output mode for every pin
 pinMode(GREEN, OUTPUT);
 pinMode(BLUE, OUTPUT);
 x = 0;                       // Initialize x
 rgb(0, 0, 0);                // Set initial state of LED to RGB #000000 (not lit)
}

// Method to set desired color on RGB LED
// Accepts three bytes r, g and b for every color
// 0 - black, 255 - maximal brightness
void rgb(byte r, byte g, byte b)
{
  //Output color in HEX format for debugging purposes
  Serial.print("#");
  Serial.print(r, HEX);
  Serial.print(g, HEX);
  Serial.print(b, HEX);
  Serial.println("");

  // Common anode LED is used, so to have full brightness of color we need to send 0 to corresponding pin
  // If we use common cathode LED, we should remove "255 -" from next lines: analogWrite(RED, r);
  analogWrite(RED, 255 - r);
  analogWrite(GREEN, 255 - g);
  analogWrite(BLUE, 255 - b);
}

void loop()
{
  // Calculate components using shifted sinus functions multiplied and lifted by amplitude.
  byte r = (byte)constrain(sin(x) * AMPLITUDE + AMPLITUDE, 0, 255);
  byte g = (byte)constrain(sin(x + 2.094) * AMPLITUDE + AMPLITUDE, 0, 255);
  byte b = (byte)constrain(sin(x + 4.189) * AMPLITUDE + AMPLITUDE, 0, 255);

  // Output x
  Serial.print("x=");
  Serial.print(x, DEC);
  Serial.print("\t");

  // Use our method to setup PWM pins
  rgb(r, g, b);

  // Wait a little
  delay(DELAYTIME);

  // Increase x for next iteration
  x += DELTAX;

  // sin(x + 2 * pi) = sin(x), so after 2 * pi we could just start over to avoid overflow and weird behaviour.
  if(x >= 6.28)
    x = 0;
}
