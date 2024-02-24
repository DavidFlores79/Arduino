#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

const char* ssid = "MIHOGAR";                                              //Enter SSID
const char* password = "e256262024";                                       //Enter Password
const char* websockets_server_host = "itsoft-websocket-server.glitch.me";  //Enter server adress
const uint16_t websockets_server_port = 80;                                // Enter server port

unsigned long previousMillis = 0;  
const long interval = 30000;       


using namespace websockets;
WebsocketsClient client;

void onMessageCallback(WebsocketsMessage message) {
  // Serial.print("Got Message: ");
  Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
    client.connect(websockets_server_host, websockets_server_port, "/");
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}


void setup() {
  Serial.begin(115200);
  // Connect to wifi
  WiFi.begin(ssid, password);

  // Wait some time to connect to wifi
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(1000);
  }

  // run callback when messages are received
  client.onMessage(onMessageCallback);

  // run callback when events are occuring
  client.onEvent(onEventsCallback);

  // Connect to server
  client.connect(websockets_server_host, websockets_server_port, "/");

  // Send a message
  client.send("ESP8266 dijo: Hola a todos!");

  // Send a ping
  //client.ping();
}

void loop() {
  client.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    String miTexto = (String)currentMillis;
    client.send("ESP8266 dijo: " + miTexto + " Hola a todos!");    
  }
}