#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFiMulti.h>
#include <Adafruit_Sensor.h>

#define SERVER_IP "sensores.enlacetecnologias.mx/api/login"
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

    String uid = getUID();

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"uid\":\"" + uid + "\"}");
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

        //imprimir el JSON del response
        for (JsonPair kv : responseJson) {
          // if(kv.value().as<String>() == "DHT11") {
            Serial.println(kv.key().c_str());
            Serial.println(kv.value().as<String>());
          // }
        }

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

void inicializandoWIFI() {

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("MIHOGAR", "e256262024");
  wifiMulti.addAP("INFINITUM5537_5", "NuFRn5uTe4");
  wifiMulti.addAP("Redmi 9", "e256262024");
  wifiMulti.addAP("FamFlores", "e256262024");
}

void imprimirConexion() {
  Serial.print("WiFi connected: ");
  Serial.print(WiFi.SSID());
  Serial.print(" ");
  Serial.println(WiFi.localIP());
}