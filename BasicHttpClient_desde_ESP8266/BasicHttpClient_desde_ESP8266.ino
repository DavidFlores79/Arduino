#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("dlink", "54146aaf13a2cb9b1837b3e260841e3e1dab67fe059e097a6a0875495401a0d6");

}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://arduino.enlacetecnologias.mx/api/users")) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          
          const char* payload = http.getString().c_str();
          Serial.println(payload);
          Serial.println("---------------------------");
          
          
          //const size_t capacity = JSON_ARRAY_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 230;
          //DynamicJsonDocument doc(capacity);
          DynamicJsonDocument doc(2048);
          
          deserializeJson(doc, payload);


          Serial.println("---------------------------");
          Serial.println(doc.size());

          for (int i = 0; i < doc.size(); i++) {
              JsonObject root_i = doc[i];
              int root_i_id = root_i["id"]; // 1
              const char* root_i_name = root_i["name"]; // "David"
              const char* root_i_email = root_i["email"]; // "davidflorescastillo@gmail.com"
              const char* root_i_created_at = root_i["created_at"]; // "2020-11-16 15:56:51"
             
              Serial.print("Nombre: ");
              Serial.println(root_i_name);
              Serial.print("Email: ");
              Serial.println(root_i_email);
              Serial.print("Cliente Desde: ");
              Serial.println(root_i_created_at);

          }
        }
      } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  delay(60000);
}
