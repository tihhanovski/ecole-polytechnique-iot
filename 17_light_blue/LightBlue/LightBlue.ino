/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * I've Got A Light Case of the Blue(tooth)s
 * Ilja Tihhanovski

*/

#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

const uint8_t MAX_BUFFER_SIZE = 8;  // Maximum size of bytes we wait from Serial/BLE

// Configuring the LEDs
const uint8_t REDPIN = 2;
const uint8_t YELLOWPIN = 3;
const uint8_t GREENPIN = 4;

const uint8_t RX1 = 10;
const uint8_t TX1 = 9;
const uint8_t STATEPIN =  8;


// First version used String object for status and commands
// With Strings compiled sketch took 8736 bytes
// With Strings replaced with char* and little optimizations sketch compressed to 6886 bytes
// I tried to replace Serial with SoftwareSerial (compiled sketch became much smaller), but unfortunately lost the ability to read data from USB, output to USB was fine

// Define possible commands
const char* CMD_RED = "red";
const char* CMD_YELLOW = "yellow";
const char* CMD_GREEN = "green";
const char* CMD_STATUS = "status";

// Color change commands
const char* STATUS_ARRAY[] = {CMD_RED, CMD_YELLOW, CMD_GREEN};


//LCD screen I2C slave id = 0x27 (= 39)
const uint8_t LCD_DEVICE_ID = 0x27;

SoftwareSerial bleSerial(RX1, TX1);
LiquidCrystal_I2C lcd(LCD_DEVICE_ID, 16, 2); // (characters per line, #lines)

uint8_t status;             // We will be using LED pin number as status
boolean connected = false;  // True if something is connected to BLE module

void setup() {

  //LEDs' pins
  pinMode(REDPIN, OUTPUT);
  pinMode(YELLOWPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);

  //Serial setup
  Serial.begin(9600);
  while(!Serial) {}

  //Bluetooth module setup
  pinMode(STATEPIN, INPUT);
  bleSerial.begin(9600);
  while (!bleSerial) {}

  lcd.init();
  lcd.backlight(); // turns on and off the backlight of the display

  // Output sketch data
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__);
  Serial.println("");

  // Output intro message to LCD
  lcd.setCursor(0, 0);
  lcd.print("Hello");
  lcd.setCursor(0, 1);
  lcd.print("Traffic!");  

  // Turn all lights on
  digitalWrite(REDPIN, HIGH);
  digitalWrite(YELLOWPIN, HIGH);
  digitalWrite(GREENPIN, HIGH);

  delay (1000);

  // Leave only red on
  digitalWrite(YELLOWPIN, LOW);
  digitalWrite(GREENPIN, LOW);
  setStatus(REDPIN);  // We use it here to output status data to LCD

  lcd.setCursor(0, 0);
  lcd.print("BLE: just booted!");
}

// Returns current status name
char* getStatusName() {
  // status array have indices from 0 to 2
  // Here we use thing that LED pins are consequent.
  // In other case we might need more complicated code
  return STATUS_ARRAY[status - REDPIN]; 
}

// Sets the status according to current pin number
void setStatus(uint8_t newStatus)
{
  if(newStatus == REDPIN || newStatus == YELLOWPIN || newStatus == GREENPIN) // Maybe a little bit too defensive programming
    if(status != newStatus)
    {
      digitalWrite(status, LOW);            // Previous color OFF
      digitalWrite(newStatus, HIGH);        // Current color (e.g. status) ON
      status = newStatus;                   // Save the current status
      char* statusName = getStatusName();   // Retrieve the current status name

      // Output current status name to BLE and LCD
      bleSerial.print(statusName);
      lcd.setCursor(0, 1);
      lcd.print("                ");        // To clear the row
      lcd.setCursor(0, 1);
      lcd.print("Color: ");
      lcd.print(statusName);
    }
}

// Just output BLE connection information to LCD
inline void updateBLEStatus()
{
  lcd.setCursor(0, 0);
  lcd.print("BLE ");
  lcd.print(connected ? "connected   " : "disconnected");  
}

void loop() {

  // Buffer for commands, we will use it as c_str, so last byte is always \0
  char buffer[MAX_BUFFER_SIZE] = "\0\0\0\0\0\0\0\0";
  uint8_t bufferSize = 0; // Buffer size

  // We will process only one source of commands at one time
  // We will ignore any remainder of the data longer than MAX_BUFFER_SIZE
  if (Serial.available() > 0) {                                       // If something was sent from Serial
    Serial.print("Have data from USB... ");
    while (Serial.available() > 0) {                                  // Until we read all that was sent
      char c = Serial.read();                                         // Read the char
      if ((c != 10) && (c != 13 ) && (bufferSize < MAX_BUFFER_SIZE))  // Ignore CR and LF and prevent buffer overflow
        buffer[bufferSize++] = c;                                     // Add incoming byte to our buffer
      delay(100);                                                     // Wait a little
    }
  } else {
    if (bleSerial.available() > 0) {                                  // If something was sent from BLE
      Serial.print("Have data from bluetooth... ");
      while (bleSerial.available() > 0) {                             // Until we read all that was sent
        char c = bleSerial.read();                                    // Read the char
        if(bufferSize < MAX_BUFFER_SIZE)                              // Prevent buffer overflow
          buffer[bufferSize++] = c;                                   // Add incoming byte to our buffer
        delay(100);                                                   // Wait a little
      }
    }
  }

  if(bufferSize)                                                      // If we have something in our buffer
  {
    Serial.print("Got a command: '");
    Serial.print(buffer);
    Serial.println("'");
  
    for(uint8_t i = 0; i < 3; i++)                                    // Check if it was "color change command"
      if(strcmp(STATUS_ARRAY[i], buffer) == 0)
        setStatus(REDPIN + i);                                        // And change status/color if it was one of them
        
    if (strcmp(buffer, CMD_STATUS) == 0)                              // If status requested
      bleSerial.print(getStatusName());                               // Output current color name
  }

  // Check if connection was made or finished since the last loop and if it was, act accordingly
  if ( (!connected) && (digitalRead(STATEPIN) == HIGH) ){
    Serial.println("A peripheral connected via Bluetooth/LE");
    connected = true;
    updateBLEStatus();
  }
  if ( (connected) && (digitalRead(STATEPIN) == LOW) ){
    Serial.println("A peripheral disconnected from Bluetooth/LE");
    connected = false;
    updateBLEStatus();
  }  
}
