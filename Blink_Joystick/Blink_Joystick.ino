/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  You can receive x and y coords for joystick movement within App.

  App project setup:
    Two Axis Joystick on V1 in MERGE output mode.
    MERGE mode means device will receive both x and y within 1 message
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int IN3 = 12;    // Input3 conectada al pin D6
int IN4 = 13;    // Input4 conectada al pin D7
int ENB = 14;    // ENB conectada al pin D5
int pos = 0;    // variable to store the servo position
int vel = 0;    // variable to store the servo position

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "0d2c6a79f48a4578b77c27a389f4fe32";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "TALLERET";
char pass[] = "2664986EBDDED936";

void moveControl(int x)
{
  // Do something with x and y
  Serial.print("X = ");
  Serial.print(x);
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
   myservo.attach(2);  // attaches the servo on pin 9 to the servo object
   myservo.write(90);
   pinMode (ENB, OUTPUT); 
   pinMode (IN3, OUTPUT);
   pinMode (IN4, OUTPUT);
   //Preparamos la salida para que el motor gire en un sentido
   digitalWrite (IN3, HIGH);
   digitalWrite (IN4, LOW);

}

void loop()
{
  Blynk.run();
}

BLYNK_WRITE(V0)
{   
  int w = param[1].asInt(); // Get value as integer
  Serial.print("; W = ");
  Serial.println(w); 
  vel = map(w,520,1023,100,300);
  analogWrite(ENB,vel);
}
BLYNK_WRITE(V1) {
  int x = param[0].asInt();
  Serial.print("; X = ");
  Serial.println(x); 
  pos = map(x,0,1023,103,75);
  myservo.write(pos);    
}
