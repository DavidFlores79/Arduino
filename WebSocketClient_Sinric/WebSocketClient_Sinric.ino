/*
 Version 0.1 - Feb 10 2018
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries 
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "xxxxx" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "MIHOGAR" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "e256262024" // TODO: Change to your Wifi network password

#define WS_SERVER "192.168.1.108"
#define PORT 5000
#define HEARTBEAT_INTERVAL 18756 // 5 Minutes 

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void turnOn(String deviceId) {
  if (deviceId == "159263") // Device ID of first device
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId); 
    digitalWrite(LED_BUILTIN, LOW); 
  } 
  // else if (deviceId == "159263") // Device ID of second device
  // { 
  //   Serial.print("Turn on device id: ");
  //   Serial.println(deviceId);
  // }
  // else {
  //   Serial.print("Turn on for unknown device id: ");
  //   Serial.println(deviceId);    
  // }     
}

void turnOff(String deviceId) {
   if (deviceId == "159263") // Device ID of first device
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);  
     digitalWrite(LED_BUILTIN, HIGH);    
   }
  //  else if (deviceId == "159263") // Device ID of second device
  //  { 
  //    Serial.print("Turn off Device ID: ");
  //    Serial.println(deviceId);
  // }
  // else {
  //    Serial.print("Turn off for unknown device id: ");
  //    Serial.println(deviceId);    
  // }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WS] Intentando conectar con el Servidor WebSocket!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WS] Servicio conectado con el Servidor WebSocket!: %s\n", payload);
      Serial.printf("Esperando informacion del Servidor ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WS] Mensaje Recibido: %s\n", payload);
        // Example payloads

        // For Light device type
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html
        // {"deviceId": xxxx, "action": "AdjustBrightness", value: 3} // https://developer.amazon.com/docs/device-apis/alexa-brightnesscontroller.html
        // {"deviceId": xxxx, "action": "setBrightness", value: 42} // https://developer.amazon.com/docs/device-apis/alexa-brightnesscontroller.html
        // {"deviceId": xxxx, "action": "SetColor", value: {"hue": 350.5,  "saturation": 0.7138, "brightness": 0.6501}} // https://developer.amazon.com/docs/device-apis/alexa-colorcontroller.html
        // {"deviceId": xxxx, "action": "DecreaseColorTemperature"} // https://developer.amazon.com/docs/device-apis/alexa-colortemperaturecontroller.html
        // {"deviceId": xxxx, "action": "IncreaseColorTemperature"} // https://developer.amazon.com/docs/device-apis/alexa-colortemperaturecontroller.html
        // {"deviceId": xxxx, "action": "SetColorTemperature", value: 2200} // https://developer.amazon.com/docs/device-apis/alexa-colortemperaturecontroller.html
        
#if ARDUINOJSON_VERSION_MAJOR == 5
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
#endif
#if ARDUINOJSON_VERSION_MAJOR == 6        
        DynamicJsonDocument json(1024);
        deserializeJson(json, (char*) payload);      
#endif  
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"}      
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else if(value == "OFF") {
                turnOff(deviceId);
            }
        }
        else if(action == "SetColor") {
            // Alexa, set the device name to red
            // get text: {"deviceId":"xxxx","action":"SetColor","value":{"hue":0,"saturation":1,"brightness":1}}
            String hue = json ["value"]["hue"];
            String saturation = json ["value"]["saturation"];
            String brightness = json ["value"]["brightness"];

            Serial.println("[WS] hue: " + hue);
            Serial.println("[WS] saturation: " + saturation);
            Serial.println("[WS] brightness: " + brightness);
        }
        else if(action == "SetBrightness") {
          
        }
        else if(action == "AdjustBrightness") {
          
        }
        else if (action == "test") {
            Serial.println("[WS] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WS] get binary length: %u\n", length);
      break;
    default: break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 
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
  webSocket.begin(WS_SERVER, PORT, "/");

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
          // webSocket.sendTXT("H");  
          webSocket.sendTXT("{\"userId\":\"ESP8266\",\"message\":\"Hola mundo desde ESP8266\",\"date\":\"21:30:48\",\"is_heartbeat\":false,\"is_server\":false,\"color\":\"green\"}");       
      }
  }   
}
