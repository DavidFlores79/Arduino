#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
 
const char* ssid = "dlink";
const char* password =  "54146aaf13a2cb9b1837b3e260841e3e1dab67fe059e097a6a0875495401a0d6";
 
const char * headerKeys[] = {"name", "email"} ;
const size_t numberOfHeaders = 2;
 
void setup () {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.println("Connecting..");
 
  }
 
  Serial.println("Connected to WiFi Network");
}
 
void loop() {
 
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;
 
    http.begin("http://arduino.enlacetecnologias.mx/api/users"); 
 
    http.collectHeaders(headerKeys, numberOfHeaders);
 
    int httpCode = http.GET();
 
    if (httpCode > 0) {
 
      for(int i = 0; i< http.headers(); i++){
        Serial.println(http.header(i));
      }
 
      String headerDate = http.header("name");
      Serial.println(headerDate);                     
 
      String headerServer = http.header("email");
      Serial.println(headerServer); 
 
      Serial.println("--------------------");
 
    }else{
      Serial.println("An error occurred sending the request");
    }
 
    http.end();
 
  }
 
  delay(10000);
 
}
