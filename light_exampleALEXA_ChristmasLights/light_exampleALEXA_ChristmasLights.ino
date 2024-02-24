/*
 Version 0.4 - April 26 2019
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries (use the correct version)
#include <StreamString.h>
#define LED_BUILTIN 2
#define LED_SWITCH 12


ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 2000;           // interval at which to blink (milliseconds)
int i = 0;
boolean ledState = HIGH;             // ledState used to set the LED


#define MyApiKey "73ced3c7-64bc-4cdc-a508-146361a36432" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "MIHOGAR" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "e256262024" // TODO: Change to your Wifi network password

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;


// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId, String value) {
  if (deviceId == "5dfd846d22a7867b6ceee479") // Device ID of first device
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    i = 1;
  } 
  else if (deviceId == "5xxxxxxxxxxxxxxxxxxxxxxxxx") // Device ID of second device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
  }
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId, String value) {
   if (deviceId == "5dfd846d22a7867b6ceee479") // Device ID of first device
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     i = 0;
   }
   else if (deviceId == "5axxxxxxxxxxxxxxxxxxx") // Device ID of second device
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
  }
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github
#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
#endif        
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId, value);
            } else {
                turnOff(deviceId, value);
            }
        }
        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_SWITCH, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); //primer estado APAGADO
  digitalWrite(LED_SWITCH, HIGH); //primer estado APAGADO

  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();

  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }

      unsigned long currentMillis = millis();
    
      if (currentMillis - previousMillis >= interval && i != 0) {
        
        previousMillis = currentMillis;
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        digitalWrite(LED_SWITCH, ledState);
        digitalWrite(LED_BUILTIN, !ledState);
      }
      if(i == 0){
        digitalWrite(LED_SWITCH, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);
        }
  }   
}
