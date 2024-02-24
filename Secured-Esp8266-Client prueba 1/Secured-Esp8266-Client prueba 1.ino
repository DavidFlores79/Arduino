#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>

const char* ssid = "MIHOGAR"; //Enter SSID
const char* password = "e256262024"; //Enter Password

const char* websockets_connection_string = "wss://itsoft-websocket-server.glitch.me/"; //Enter server adress

// This latest SHA1 fingerprint was updated 15.04.2021
const char echo_org_ssl_fingerprint[] PROGMEM   = "34 A2 66 08 A1 4D 1E 83 1A 0E 49 3C 4A 84 45 9E 4A 0D 08 FE";

using namespace websockets;

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

WebsocketsClient client;
void setup() {
    Serial.begin(115200);
    // Connect to wifi
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    // run callback when messages are received
    client.onMessage(onMessageCallback);
    
    // run callback when events are occuring
    client.onEvent(onEventsCallback);

    // Before connecting, set the ssl fingerprint of the server
    client.setFingerprint(echo_org_ssl_fingerprint);

    // Connect to server
    client.connect(websockets_connection_string);

    // Send a message
    client.send("Hello Server");

    // Send a ping
    //client.ping();
}

void loop() {
    client.poll();
}
