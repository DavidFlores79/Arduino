#include <ESP8266WiFi.h>

const char* ssid = "TALLERPLAY";
const char* password = "2664986EBDDED936";
bool flag_flushed = false;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(23);

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
 
 
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  if (!flag_flushed)
  {
    client.flush();
    flag_flushed = true;
  }
 
  if (client.available() > 0)
  {
    char byteread = client.read();
    Serial.write(byteread);
  }
  if (Serial.available() > 0)
  {
    char byteread = Serial.read();
    client.write(byteread);   
  }
}
