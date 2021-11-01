//See https://create.arduino.cc/projecthub/pibots555/how-to-connect-dht11-sensor-with-arduino-uno-f4d239

// Code partly taken from example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor


#include "DHT.h"

#define DHTPIN 2        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define MAX_DELAY 10000

DHT dht(DHTPIN, DHTTYPE);

float humidity;
float temperature;
float heatIndex;
unsigned long time;

void setup() {
  Serial.begin(9600);
  Serial.println("boot");

  dht.begin();
  time = 0;
}


void loop() {
  delay(2000);
  

  // Reading temperature or humidity takes about 250 milliseconds
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("fail");
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  if((humidity != h) || (temperature != t) || (heatIndex != hic) || (time < millis()))
  {
    time = millis() + MAX_DELAY;
    humidity = h;
    temperature = t;
    heatIndex = hic;
    
    Serial.print("t:");
    Serial.print(temperature);
    Serial.print("\th:");
    Serial.print(humidity);
    Serial.print("\ti:");
    Serial.println(heatIndex);
  }
}
