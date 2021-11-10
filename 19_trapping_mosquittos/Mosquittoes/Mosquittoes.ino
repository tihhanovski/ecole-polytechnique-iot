/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * MQTT: Trapping mosquitoes
 * Ilja Tihhanovski
 * 
 * Code partly taken from example testing sketch for various DHT humidity/temperature sensors
 * Created by Pi BOTS MakerHub 
 * See https://create.arduino.cc/projecthub/pibots555/how-to-connect-dht11-sensor-with-arduino-uno-f4d239
 */

#include "DHT.h"

#define DHTPIN 2        // Pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define MAX_DELAY 10000

DHT dht(DHTPIN, DHTTYPE);

// Here we store values obtained from the sensor
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
  delay(2000);  // Could be replaced with watchdog timer and sleeping mode to save energy
  
  // Read temperature and humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println("fail");
    return;
  }

  // Compute heat index in Celsius
  float hic = dht.computeHeatIndex(t, h, false);

  // If any of readings canged or long time passed after last data sent
  if((humidity != h) || (temperature != t) || (heatIndex != hic) || (time < millis()))
  {
    time = millis() + MAX_DELAY;        // Set next last time to send readings
    humidity = h;                       // Save the values
    temperature = t;
    heatIndex = hic;

    // Output data to Serial
    Serial.print("t:");
    Serial.print(temperature);
    Serial.print("\th:");
    Serial.print(humidity);
    Serial.print("\ti:");
    Serial.println(heatIndex);
  }
}
