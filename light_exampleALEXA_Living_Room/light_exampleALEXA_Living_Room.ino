#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h>
#include "EmonLib.h"

#define RatioTC 2000
#define ResCarga 91
#define VoltajeLinea 117

EnergyMonitor emon1;
float effective_value; //effective current
int current_sensor_pin = A0;// A0=GPIO17 Analog input pin that sensor is attached to
int device_on_current_threshold = 8; //if the current value (milliamps) is greater than this amount then the device is on
bool deviceState = 0;        // current state of the switch
int lastDeviceState = 0;    // previous state of the switch

//para las notificaciones directas cada medio segundo
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 500;           // interval at which to blink (milliseconds)

#define API_KEY "73ced3c7-64bc-4cdc-a508-146361a36432" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define SSID_NAME "MIHOGAR" // TODO: Change to your Wifi network SSID
#define WIFI_PASSWORD "e256262024" // TODO: Change to your Wifi network password
#define SERVER_URL "iot.sinric.com"
#define SERVER_PORT 80 

#define HEARTBEAT_INTERVAL 60000 // 1 Minuto 

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // Indicamos dónde está conectado el sensor de corriente (pin analógico A0) y el factor de calibración.
  emon1.current(current_sensor_pin, RatioTC / ResCarga);
  double Irms = emon1.calcIrms(2480);

  WiFiMulti.addAP(SSID_NAME, WIFI_PASSWORD);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(SSID_NAME);  

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
  webSocket.begin(SERVER_URL, SERVER_PORT, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", API_KEY);
  
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
            setPowerStateOnServer("5bf2d58537c3d52b8427d29f", "ON");
            Serial.println("Sending device status ON id: 5bf2d58537c3d52b8427d29f");
          }else{
            setPowerStateOnServer("5bf2d58537c3d52b8427d29f", "OFF");
            Serial.println("Sending device status OFF id: 5bf2d58537c3d52b8427d29f");
          }
                    
      }
  }   




}
  
void turnOn(String deviceId) {
  if (deviceId == "5bf2d58537c3d52b8427d29f") // Device ID of first device
  {  
    currentMeter();
    int currentRead = digitalRead(LED_BUILTIN);
    if((deviceState == 0) && (digitalRead(LED_BUILTIN) == 1)){
      digitalWrite(LED_BUILTIN, LOW);
      Serial.print("Turn on device id: ");
      Serial.println(deviceId);
    }else if((deviceState == 0) && (digitalRead(LED_BUILTIN) == 0)){
      digitalWrite(LED_BUILTIN, HIGH);  
      Serial.print("2 Turn on device id: ");
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
   if (deviceId == "5bf2d58537c3d52b8427d29f") // Device ID of first device
   {  
    currentMeter();
    int currentRead = digitalRead(LED_BUILTIN);
    if((deviceState == 1) && (digitalRead(LED_BUILTIN) == 0)){
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.print("Turn off device id: ");
      Serial.println(deviceId);  
    }else if((deviceState == 1) && (digitalRead(LED_BUILTIN) == 1)){
      digitalWrite(LED_BUILTIN, LOW);  
      Serial.print("2 Turn off device id: ");
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
      Serial.printf("[webSocketEvent] Webservice disconnected from server!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[webSocketEvent] Service connected to server at url: %s\n", payload);
      Serial.printf("[webSocketEvent] Waiting for commands from server ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[webSocketEvent] get text: %s\n", payload);
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
      }
      break;
    case WStype_BIN:
      Serial.printf("[webSocketEvent] get binary length: %u\n", length);
      break;
  }
}


// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")

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
    double Irms = emon1.calcIrms(2480);
    effective_value = Irms * VoltajeLinea;
      if(effective_value > device_on_current_threshold){
        deviceState = 1 ;
      }else{
        deviceState = 0 ;
      }
    
}
