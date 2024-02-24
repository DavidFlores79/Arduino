#include <ArduinoJson.h>

/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "dlink";
const char* password = "54146aaf13a2cb9b1837b3e260841e3e1dab67fe059e097a6a0875495401a0d6";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://api-blog.enlacetecnologias.mx/api/register";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 600000;

void setup() {
  Serial.begin(115200);

  getConected();

  if(WiFi.status()== WL_CONNECTED){
        getRegister();
  }
  else {
    Serial.println("WiFi Disconnected");
    getConected();
  }

  
}


void loop() {
  
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    lastTime = millis();
  }
}

void getConected() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}


void getRegister(){
/* HACER LA PETICION POR POST*/
    //Check WiFi connection status
      HTTPClient http;
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.addHeader("Content-Length", "3495");
      http.addHeader("Host", serverName);

      // Data to send with HTTP POST
      String httpRequestData = "json={\"name\":\"ESP8266\",\"surname\":\"Laravel Blog\",\"email\":\"esp8266@enlacetecnologias.mx\",\"password\":\"123456\"}";
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      /*
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Content-Length", "3495");
      http.addHeader("Host", serverName);
      int httpResponseCode = http.POST("{\"name\":\"ESP8266\",\"surname\":\"Laravel Blog\",\"email\":\"esp8266@enlacetecnologias.mx\",\"password\":\"123456\"}");
      */

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");

      
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      // with API
      Serial.print("Api: ");
      Serial.println(http.getString());

      // Code (after JSON submission
      /*
      StaticJsonBuffer<500> jsonBuffer;
      
      JsonObject& root = jsonBuffer.parseObject(http.getString());
      
      const char* api_key = root["api_key"];
      
      Serial.print("Api Key return element = ");
      Serial.println(api_key);
      */
        
      // Free resources
      http.end();
}
