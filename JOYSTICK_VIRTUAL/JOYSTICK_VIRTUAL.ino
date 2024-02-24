/* JOYSTICK VIRTUAL

 Para su funcionamiento es necesario guardar el fichero index.html 
 (http://www.esploradores.com/wp-content/uploads/2017/03/index.zip) 
 en el Sistema de Archivos para Memoria Flash (SPIFF).

 Esto se realiza utilizando el complemento del IDE de Arduino ESP8266FS  
 (https://github.com/esp8266/arduino-esp8266fs-plugin/releases/download/0.3.0/ESP8266FS-0.3.0.zip)

 Este código de ejemplo es de público dominio.
 Dani No www.esploradores.com   */

#include <ESP8266WiFi.h>                      //Librería de conexión WiFi del módulo ESP8266
#include <ESP8266WebServer.h>                 //Librería ESP8266WebServer (simplifica la creación del servidor)
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int IN3 = 12;    // Input3 conectada al pin D6
int IN4 = 13;    // Input4 conectada al pin D7
int ENB = 14;    // ENB conectada al pin D5
int pos = 0;    // variable to store the servo position
int vel = 0;    // variable to store the servo position

const char *ssid = "JOYSTICK_VIRTUAL",        // Credenciales del Punto de Acceso
           *password = "12345678";               

ESP8266WebServer server(80);            

void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  server.on("/stick", []() {                  // Recibe los datos (dx y dy)
    String dx=server.arg("dx");
    String dy=server.arg("dy");
    server.send(200);
    int x = dx.toInt();                       // Convierte los datos de tipo String a Int      
    int y = dy.toInt();
    Serial.print(x);  Serial.print("  ");  Serial.println(y);  
    pos = map(x,0,255,103,75);
    myservo.write(pos);    
    vel = map(y,176,0,0,500);
    analogWrite(ENB,vel);

  }); 

   myservo.attach(2);  // attaches the servo on pin 9 to the servo object
   myservo.write(90);
   pinMode (ENB, OUTPUT); 
   pinMode (IN3, OUTPUT);
   pinMode (IN4, OUTPUT);
   //Preparamos la salida para que el motor gire en un sentido
   digitalWrite (IN3, HIGH);
   digitalWrite (IN4, LOW);

}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

