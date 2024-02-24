#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>
#include "EmonLib.h"                   // Include Emon Library
#define device_on_current_threshold 1500

EnergyMonitor emon1;                   // Create an instance

float effective_value; //effective current
bool deviceState = 0;        // current state of the switch

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "73ced3c7-64bc-4cdc-a508-146361a36432" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "MIHOGAR" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "e256262024" // TODO: Change to your Wifi network password

#define HEARTBEAT_INTERVAL 60000 // 1 Minute

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

//5c911a8d7428a644f13625a9 es la ID para CUARTO PRINCIPAL
 
void turnOn(String deviceId) {
  if (deviceId == "5c911a8d7428a644f13625a9") // Device ID of first device
  {  
    currentMeter();
    int currentRead = digitalRead(LED_BUILTIN);
    if((deviceState == 0) && (digitalRead(LED_BUILTIN) == 1)){
      digitalWrite(LED_BUILTIN, LOW);
      Serial.print(" Turn on device id: ");
      Serial.println(deviceId);
    }else if((deviceState == 0) && (digitalRead(LED_BUILTIN) == 0)){
      digitalWrite(LED_BUILTIN, HIGH);  
      Serial.print(" 2 Turn on device id: ");
      Serial.println(deviceId);  
    }else{
      Serial.print(" Status: ");
      Serial.print(deviceState);
      Serial.print(" Led Builtin: ");
      Serial.println(currentRead);
    }
  }
}

void turnOff(String deviceId) {
   if (deviceId == "5c911a8d7428a644f13625a9") // Device ID of first device
   {  
    currentMeter();
    int currentRead = digitalRead(LED_BUILTIN);
    if((deviceState == 1) && (digitalRead(LED_BUILTIN) == 0)){
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.print(" Turn off device id: ");
      Serial.println(deviceId);  
    }else if((deviceState == 1) && (digitalRead(LED_BUILTIN) == 1)){
      digitalWrite(LED_BUILTIN, LOW);  
      Serial.print(" 2 Turn off device id: ");
      Serial.println(deviceId);  
    }else{
      Serial.print(" Status: ");
      Serial.print(deviceState);
      Serial.print(" Led Builtin: ");
      Serial.println(currentRead);
    }
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

        // For Switch  types
        // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":true}} // https://developers.google.com/actions/smarthome/traits/onoff
        // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":false}}

        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") {
            // alexa, turn on tv ==> {"deviceId":"xx","action":"setPowerState","value":"ON"}
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }        
        }
        else if(action == "action.devices.commands.OnOff") {
            // ok google, enciende/apaga living room
            String value = json ["value"]["on"];
            if(value == "true") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
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

  // Indicamos dónde está conectado el sensor de corriente (pin analógico A0) y el factor de calibración.
  emon1.current(A0, 111.1);             // Current: input pin, calibration.

  for(int i=0; i<=100; i++){
    currentMeter();
    delay(50);
  }
  
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
  webSocket.begin("iot.sinric.com", 80, "/"); //"iot.sinric.com", 80

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
          currentMeter();
          if (deviceState == 1) {
            setPowerStateOnServer("5c911a8d7428a644f13625a9", "ON");
            Serial.println("Sending device status ON id: 5bf2d58537c3d52b8427d29f");
          }else{
            setPowerStateOnServer("5c911a8d7428a644f13625a9", "OFF");
            Serial.println("Sending device status OFF id: 5bf2d58537c3d52b8427d29f");
          }


      }
  }   
}

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 
void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}

void currentMeter()
{
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    effective_value = Irms * 960;
      if(effective_value > device_on_current_threshold){
        deviceState = 1 ;
      }else{
        deviceState = 0 ;
      }
      Serial.println(effective_value);
    
}
