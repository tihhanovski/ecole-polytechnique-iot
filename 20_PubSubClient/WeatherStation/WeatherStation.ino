/*
 * INF471C - Connected Objects and the Internet of Things (IoT)
 * PubSubClient and ESP8266
 * Ilja Tihhanovski
 *  
 * Examples used
 * https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
 * https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

//#define WIFI_SSID "Tartu Kolledž"
//#define WIFI_PASS "Puie5tee"

#define WIFI_SSID "Telia-86EB42"
#define WIFI_PASS "EMPBNVJUMMHXRR"

//#define WIFI_SSID "Intellisoft_public"
//#define WIFI_PASS "Profit201704"

//#define WIFI_SSID "POCO X3 Pro"
//#define WIFI_PASS "nedzfzchvxb9bcy"

#define MQTT_SERVER "dev.intellisoft.ee"
#define MQTT_CLIENT "ESP32Client"
#define MQTT_USER "ilja"
#define MQTT_PASS "inf471c"
#define MQTT_PORT 1883
#define LED_PIN 2
#define DEF_TIME_BETWEEN_PUBS 2000 // in milliseconds
#define MIN_TIME_BETWEEN_PUBS_SEC 1 // in seconds
#define MAX_TIME_BETWEEN_PUBS_SEC 60 // in seconds
// Limit maximum command from MQTT, that could be processed
#define MAX_COMMAND_LENGTH 50

#define DHTPIN 4        // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

const char* commandTopic = "esp32/command";

WiFiClient wifi;
PubSubClient mqtt(wifi);

DHT dht(DHTPIN, DHTTYPE);

int cnt = 0;
unsigned long nextPubMillis = 0;

float humidity;
float temperature;
float heatIndex;

bool requestBlink = false;
unsigned long timeBetweenPubs = DEF_TIME_BETWEEN_PUBS;

// See https://techtutorialsx.com/2017/06/29/esp32-arduino-getting-started-with-wifi/
/*
String translateEncryptionType(wifi_auth_mode_t encryptionType) {
 
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}
*/

void connectToWifi()
{
  /*int numberOfNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numberOfNetworks; i++) {
 
    Serial.print(WiFi.SSID(i));
    Serial.print(":\tRSSI: ");
    Serial.print(WiFi.RSSI(i));
 
    Serial.print(";\tMAC: ");
    Serial.print(WiFi.BSSIDstr(i));
 
    Serial.print("\tEnc: ");
    Serial.println(translateEncryptionType(WiFi.encryptionType(i)));
  }
  Serial.println("-----------------------");
  */

  // Connect to a WiFi network
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
  
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial) {}

  // Connect sensor
  dht.begin();  

  connectToWifi();

  //Setup MQTT, (re)connection will be initiated from loop
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(callback);
}

const char* CMD_RESET_COUNTER = "resetCounter";
const char* CMD_BLINK = "blink";
const char* CMD_SETDELAY = "setDelay";
const size_t CMD_SETDELAY_LENGTH = 8;

void callback(char* topic, byte* message, unsigned int length) {
  size_t msgLength = (length > MAX_COMMAND_LENGTH) ? MAX_COMMAND_LENGTH : length;
  char* msg = new char[msgLength + 1];
  memcpy(msg, message, msgLength);
  msg[msgLength] = 0;  //c_str final byte

  Serial.print("Message arrived on topic: '");
  Serial.print(topic);
  Serial.print("'. Message: '");
  Serial.print(msg);  
  Serial.println("'");

  if(!strcmp(topic, commandTopic)) {
    Serial.println("process a command");

    //resetCounter sets cnt to 0
    if(!strcmp(msg, CMD_RESET_COUNTER)) {
      Serial.println("resetCounter requested");
      cnt = 0;
    }

    //blink makes controller to blink its built-in led
    if(!strcmp(msg, CMD_BLINK)) {
      Serial.println("blink");
      requestBlink = true;
    }

    // setDelay NN (where NN is integer) changes delay between publications
    // delay given in seconds.
    // If delay is out of min and max delay interval [MAX_TIME_BETWEEN_PUBS_SEC .. MAX_TIME_BETWEEN_PUBS_SEC]
    // then controller will output corresponding message to Serial and blink the built-in led
    if(!memcmp(msg, CMD_SETDELAY, CMD_SETDELAY_LENGTH)) {
      Serial.print("New delay requested ");

      //unsigned long newDelayRequested = messageTemp.substring(9).toInt();
      unsigned long newDelayRequested = atoi(msg + CMD_SETDELAY_LENGTH);
      if(newDelayRequested > MAX_TIME_BETWEEN_PUBS_SEC) {
        Serial.print("delay ");
        Serial.print(newDelayRequested);
        Serial.print(" is too long, max ");
        Serial.println(MAX_TIME_BETWEEN_PUBS_SEC);
        requestBlink = true;
        return;
      }
  
      if(newDelayRequested < MIN_TIME_BETWEEN_PUBS_SEC) {
        Serial.print("delay ");
        Serial.print(newDelayRequested);
        Serial.print(" is too short, min ");
        Serial.println(MIN_TIME_BETWEEN_PUBS_SEC);
        requestBlink = true;
        return;
      }
  
      timeBetweenPubs = newDelayRequested;
      Serial.print(timeBetweenPubs);
      Serial.println(" OK");
    }
  }
  
  delete msg; //free memory
}


void reconnect() {
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
  
  if (!mqtt.connected())
    reconnect();
  mqtt.loop();

  if(requestBlink) {
    for(uint8_t i = 0; i < 3; i++)
    {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
    requestBlink = false;
  }

  if(millis() > nextPubMillis) {
    cnt++;

    float h = dht.readHumidity();        
    float t = dht.readTemperature();    // Read temperature as Celsius (the default)

     // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("fail");
    } else {
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);
  
      if((humidity != h) || (temperature != t) || (heatIndex != hic)) {
        humidity = h;
        temperature = t;
        heatIndex = hic;

        char msg[50];
        sprintf(msg, "t:%.2f\th:%.2f\ti:%.2f\tc:%d", temperature, humidity, heatIndex, cnt);
        
        Serial.print("Publish '");
        Serial.print(msg);
        Serial.println("'");
        mqtt.publish("esp32/dht", msg);
      }
    }
    nextPubMillis = millis() + timeBetweenPubs;
  }
}
