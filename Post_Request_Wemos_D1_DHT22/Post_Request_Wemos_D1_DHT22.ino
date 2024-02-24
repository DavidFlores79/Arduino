#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN D6      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22  // DHT 22
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

#define SERVER_IP "sensores.enlacetecnologias.mx/api/datos"
ESP8266WiFiMulti wifiMulti;
const uint32_t connectTimeoutMs = 5000;

void setup() {
  Serial.begin(115200);
  inicializandoWIFI();
  inicializandoSensorDHT();
}

void loop() {
  // wait for WiFi connection
  // if ((WiFi.status() == WL_CONNECTED)) {
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {

    imprimirConexion();

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP);  //HTTP
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");

    float temperatura = getTemperatura();
    float humedad = getHumedad();

    String temp = String(temperatura);
    String hum = String(humedad);

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"sensor\":\"ArduinoDHT22\",\"temperatura\":\"" + temp + "\",\"humedad\":\"" + hum + "\"}");
    Serial.println(httpCode);
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        DynamicJsonDocument doc(1024);
        String myJson = http.getString();
        deserializeJson(doc, myJson);
        JsonObject responseJson = doc.as<JsonObject>();
        String status = responseJson["status"];

        // for (JsonPair kv : responseJson) {
        //   if(kv.value().as<String>() == "DHT11") {
        //     Serial.println(kv.key().c_str());
        //     Serial.println(kv.value().as<String>());
        //   }
        // }

        Serial.println("<<<<<<<<<<<<<<>>>>>>>>>>>>>>");
        Serial.print("status: ");
        Serial.println(status);
        Serial.println("<<<<<<<<<<<<<<>>>>>>>>>>>>>>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
  delay(300000);
}

double randomDouble(double minf, double maxf) {
  return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31);  // use 1ULL<<63 for max double values)
}

float getTemperatura() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  return event.temperature;
}

float getHumedad() {

  sensors_event_t event;
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  } else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
  return event.relative_humidity;
}

void inicializandoWIFI() {

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("MIHOGAR", "e256262024");
  wifiMulti.addAP("INFINITUM5537_5", "NuFRn5uTe4");
  wifiMulti.addAP("Redmi 9", "e256262024");
  wifiMulti.addAP("FamFlores", "e256262024");
}

void inicializandoSensorDHT() {
  dht.begin();
  Serial.println(F("Sensor inicializado"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
}

void imprimirConexion() {
  Serial.print("WiFi connected: ");
  Serial.print(WiFi.SSID());
  Serial.print(" ");
  Serial.println(WiFi.localIP());
}