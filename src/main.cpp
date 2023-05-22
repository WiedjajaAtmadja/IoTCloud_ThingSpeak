#include <Arduino.h>
#include <DHTesp.h>
#include <BH1750.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#define DHT_PIN 19

// WiFi credentials
#define WIFI_SSID "iot2.4g"
#define WIFI_PASSWORD "iot2.4g123"

// Thingspeak credentials
#define SECRET_CH_ID 2157318
#define SECRET_WRITE_APIKEY "HN7IJVUZ8ILBMD8O"

// variables
WiFiClient client;
DHTesp dht;
BH1750 lightMeter;

void WifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(3000);
    ESP.restart();
  }
  Serial.print("System connected with IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("RSSI: %d\n", WiFi.RSSI());
}

void onReadSensor()
{
    digitalWrite(LED_BUILTIN, HIGH);
    float fHumidity = dht.getHumidity();
    float fTemperature = dht.getTemperature();
    float lux = lightMeter.readLightLevel();
    Serial.printf("Humidity: %.2f, Temperature: %.2f, Light: %.2f \n",
       fHumidity, fTemperature, lux);
    ThingSpeak.setField(1, fTemperature);
    ThingSpeak.setField(2, fHumidity);
    ThingSpeak.setField(3, lux);
    int nResult = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
    Serial.printf("ThingSpeak update channel result: %d\n", nResult);
    digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  Serial.begin(115200);
  // init Leds
  pinMode(LED_BUILTIN, OUTPUT);

  dht.setup(DHT_PIN, DHTesp::DHT11);
  Wire.begin();
  lightMeter.begin(); 
  WiFi.mode(WIFI_STA);
  WifiConnect();
  ThingSpeak.begin(client);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("Connected to the WiFi network");
  }
  onReadSensor();
  delay(15000);
}

