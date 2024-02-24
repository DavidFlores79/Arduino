#include <ESP8266WiFi.h>

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 3000;           // interval at which to blink (milliseconds)

const char* ssid = "TALLERPLAY";
const char* password = "2664986EBDDED936";

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 1
WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  
  Serial.begin(115200);
  server.begin();
  server.setNoDelay(true);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to "); Serial.println(ssid);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
  if(i == 21){
    Serial.print("Could not connect to"); Serial.println(ssid);
    while(1) delay(500);
  }
 
  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");
}

void loop() {
  
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
  

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    char testdata[] = {'H','O','L','A','\n'};
    serverClients[0].write(testdata, 5);
    
  }

}
