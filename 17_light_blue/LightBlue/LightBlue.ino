#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// Configuring the LEDs
const int REDPIN = 2;
const int YELLOWPIN = 3;
const int GREENPIN = 4;

#define STATUS_RED "red"
#define STATUS_YELLOW "yellow"
#define STATUS_GREEN "green"

#define STATUS_DEFAULT STATUS_RED

#define RX1 10
#define TX1 9
#define STATEPIN 8

//LCD screen I2C slave id = 0x27 (= 39)
#define LCD_DEVICE_ID 0x27

SoftwareSerial mySoftwareSerial(RX1, TX1);
LiquidCrystal_I2C lcd(LCD_DEVICE_ID, 16, 2); // (characters per line, #lines)

String status;
boolean connected = false;

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
  mySoftwareSerial.begin(9600);
  while (!mySoftwareSerial) {}

  lcd.init(); 
  lcd.backlight(); // turns on and off the backlight of the display
  
  Serial.print("Sketch:   ");   Serial.println(__FILE__);
  Serial.print("Uploaded: ");   Serial.println(__DATE__);
  Serial.println(" ");

  lcd.setCursor(0, 0);
  lcd.print("Hello");
  lcd.setCursor(0, 1);
  lcd.print("Traffic!");  

  digitalWrite(REDPIN, HIGH);
  digitalWrite(YELLOWPIN, HIGH);
  digitalWrite(GREENPIN, HIGH);

  delay (1000);

  setStatus(STATUS_DEFAULT);

  lcd.setCursor(0, 0);
  lcd.print("BLE: just booted!");
}

void setStatus(String newStatus)
{
  if(newStatus == STATUS_RED || newStatus == STATUS_YELLOW || newStatus == STATUS_GREEN)
    if(status != newStatus)
    {
      //Serial.print("ns: ");
      //Serial.println(newStatus);
      digitalWrite(REDPIN, newStatus == STATUS_RED);
      digitalWrite(YELLOWPIN, newStatus == STATUS_YELLOW);
      digitalWrite(GREENPIN, newStatus == STATUS_GREEN);

      status = newStatus;
      mySoftwareSerial.print(status);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("Color: ");
      lcd.print(status);
    }
}

inline void updateBLEStatus()
{
  lcd.setCursor(0, 0);
  lcd.print("BLE ");
  lcd.print(connected ? "connected   " : "disconnected");  
}

void loop() {
  String command = "";
  if (Serial.available() > 0) {
    Serial.print("Have data from USB... ");
    while (Serial.available() > 0) {
      char c = Serial.read();
      if ((c != 10) & (c != 13 )) {
        command = command + c;
      }
      delay(100);
    }
    Serial.print("Got a command: '");
    Serial.print(command);
    Serial.println("'");
    setStatus(command);
  }

  if (mySoftwareSerial.available() > 0) {
    Serial.print("Have data from bluetooth... ");
    while (mySoftwareSerial.available() > 0) {
      char c = mySoftwareSerial.read();
      command = command + c;
      delay(100);
    }
    Serial.print("Got a command: '");
    Serial.print(command);
    Serial.println("'");
    setStatus(command);

    if (command == "status")
      mySoftwareSerial.print(status);
  }

  if ( (!connected) && (digitalRead(STATEPIN) == HIGH) ){
    Serial.println("A peripheral connected via Bluetooth/LE");
    connected = true;
    updateBLEStatus();
  }
  if ( (connected) && (digitalRead(STATEPIN) == LOW) ){
    Serial.println("A peripheral dis-Connected from Bluetooth/LE");
    connected = false;
    updateBLEStatus();
  }  
}
