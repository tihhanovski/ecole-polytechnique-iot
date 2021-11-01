// https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
// https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/
//

#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"


//#define WIFI_SSID "Telia-86EB42"
//#define WIFI_PASS "EMPBNVJUMMHXRR"

#define WIFI_SSID "Intellisoft_public"
#define WIFI_PASS "Profit201704"

#define MQTT_SERVER "dev.intellisoft.ee"
#define MQTT_CLIENT "ESP32Client"
#define MQTT_USER "ilja"
#define MQTT_PASS "inf471c"
#define MQTT_PORT 1883
#define LED_PIN 2
#define DEF_TIME_BETWEEN_PUBS 2000 // in milliseconds
#define MIN_TIME_BETWEEN_PUBS_SEC 1 // in seconds
#define MAX_TIME_BETWEEN_PUBS_SEC 60 // in seconds

#define DHTPIN 4        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11


WiFiClient wifi;
PubSubClient mqtt(wifi);

DHT dht(DHTPIN, DHTTYPE);

int cnt = 0;

float humidity;
float temperature;
float heatIndex;

bool requestBlink = false;
unsigned long timeBetweenBubs = DEF_TIME_BETWEEN_PUBS;



void setup() {
  pinMode(LED_PIN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);

  dht.begin();  

  //Connect to WiFi
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  

  //MQTT stuff

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(callback);
}

const char* commandTopic = "esp32/command";

void callback(char* topic, byte* message, unsigned int length) 
{
  Serial.print("Message arrived on topic: '");
  Serial.print(topic);
  Serial.print("'. Message: '");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println("'");

  /*
  Serial.println(strcmp(topic, commandTopic));
  Serial.println(strlen(topic));
  Serial.println(strlen(commandTopic));
  Serial.println("--------");
  */

  if(strcmp(topic, commandTopic) == 0)
  {
    Serial.println("process a command");
    //messageTemp.trim();
  
    //TODO process input
    if(messageTemp == "resetCounter")
    {
      Serial.println("resetCounter requested");
      cnt = 0;
    }
  
    if(messageTemp == "blink")
    {
      Serial.println("blink");
      requestBlink = true;
    }

    if(messageTemp.substring(0, 8) == "setDelay")
    {
      Serial.print("New delay requested ");
      unsigned long newDelayRequested = messageTemp.substring(9).toInt();
      if(newDelayRequested > MAX_TIME_BETWEEN_PUBS_SEC)
      {
        Serial.print("delay ");
        Serial.print(newDelayRequested);
        Serial.print(" is too long, max ");
        Serial.print(MAX_TIME_BETWEEN_PUBS_SEC);
        requestBlink = true;
        return;
      }
  
      if(newDelayRequested < MIN_TIME_BETWEEN_PUBS_SEC)
      {
        Serial.print("delay ");
        Serial.print(newDelayRequested);
        Serial.print(" is too short, min ");
        Serial.print(MIN_TIME_BETWEEN_PUBS_SEC);
        requestBlink = true;
        return;
      }
  
      timeBetweenBubs = newDelayRequested;
      Serial.print(timeBetweenBubs);
      Serial.println(" OK");
    }
  }

}

unsigned long nextPubMillis = 0;

void reconnect() 
{
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      // Subscribe
      mqtt.subscribe("esp32/command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();

  if(requestBlink)
  {
    for(uint8_t i = 0; i < 3; i++)
    {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
    requestBlink = false;
  }

  if(millis() > nextPubMillis)
  {
    cnt++;

    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

     // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("fail");
    }
    else
    {
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);
  
      if((humidity != h) || (temperature != t) || (heatIndex != hic))
      {
        humidity = h;
        temperature = t;
        heatIndex = hic;
  
        String msg = "t:" + String(temperature)
          + "\th:" + String(humidity)
          + "\ti:" + String(heatIndex)
          + "\tc:" + String(cnt);
      
        Serial.print("Sending to mqtt ");
        Serial.println(msg);
        mqtt.publish("esp32/dht", msg.c_str());
      }
    }
    nextPubMillis = millis() + timeBetweenBubs;
  }
}
