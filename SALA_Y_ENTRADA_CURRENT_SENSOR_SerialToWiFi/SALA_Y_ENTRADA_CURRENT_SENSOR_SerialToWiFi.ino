#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>
#include "EmonLib.h"                   // Include Emon Library
#define device_on_current_threshold 650

EnergyMonitor emon1;                   // Create an instance

float effective_value; //effective current
bool deviceState = 0;        // current state of the switch

//Serial to WiFi
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 3000;           // interval at which to blink (milliseconds)
#define MAX_SRV_CLIENTS 1
WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];
//Serial to WiFi


ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "73ced3c7-64bc-4cdc-a508-146361a36432" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "MIHOGAR" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "e256262024" // TODO: Change to your Wifi network password
#define MySSID2 "TALLERPLAY" // TODO: Change to your Wifi network SSID
#define MyWifiPassword2 "2664986EBDDED936" // TODO: Change to your Wifi network password


#define HEARTBEAT_INTERVAL 60000 // 1 Minute
#define SOLID_RELE 13

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

//5c911a8d7428a644f13625a9 es la ID para CUARTO PRINCIPAL
//5b6259a7e4efb94d35aa604c es la ID para COMEDOR
//5bf2d58537c3d52b8427d29f ES LA ID DE LA SALA
//5bfa2c88303590117a1f3559 es para cuarto 1
 
void turnOn(String deviceId) {
  if (deviceId == "5bf2d58537c3d52b8427d29f") // Device ID of first device
  {  
    currentMeter();
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
  } else if (deviceId == "5e66ca1bcafbba4ea4f440e8") {
      digitalWrite(SOLID_RELE, LOW);
      Serial.printf("ENTRADA Turn ON device id: %s\n", deviceId);
  }
  
}

void turnOff(String deviceId) {
   if (deviceId == "5bf2d58537c3d52b8427d29f") // Device ID of first device
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
   } else if (deviceId == "5e66ca1bcafbba4ea4f440e8") {
    digitalWrite(SOLID_RELE, HIGH);
    Serial.printf("ENTRADA Turn OFF device id: %s\n", deviceId);
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
  pinMode(SOLID_RELE, OUTPUT);

  //Serial to WiFi
  server.begin();
  server.setNoDelay(true);
  //Serial to WiFi

  

  // Indicamos dónde está conectado el sensor de corriente (pin analógico A0) y el factor de calibración.
  emon1.current(A0, 111.1);             // Current: input pin, calibration.

  for(int i=0; i<=100; i++){
    currentMeter();
    delay(50);
  }
  
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  WiFiMulti.addAP(MySSID2, MyWifiPassword2);
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
            setPowerStateOnServer("5bf2d58537c3d52b8427d29f", "ON");
            Serial.println("Sending device status ON id: 5bfa2c88303590117a1f3559");
          }else{
            setPowerStateOnServer("5bf2d58537c3d52b8427d29f", "OFF");
            Serial.println("Sending device status OFF id: 5bfa2c88303590117a1f3559");
          }


      }
  }


  uint8_t i;
  //check if there are any new clients
  if (server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial.print("New client: "); Serial.print(i);
        break;
      }
    }


  }
  
  if(serverClients[0].connected()){
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
    
        SerialToWiFi();
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
void SerialToWiFi()
{
      char ds[1];
      char buffer[6];
      currentMeter();
      dtostrf(effective_value, 6, 2, buffer);
      dtostrf(deviceState, 1, 0, ds);
      serverClients[0].write(buffer, 6);
      serverClients[0].write(' ');
      serverClients[0].write(ds, 1);
      serverClients[0].write('\n');
}
