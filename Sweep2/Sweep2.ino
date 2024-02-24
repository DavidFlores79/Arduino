/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int IN3 = 12;    // Input3 conectada al pin D6
int IN4 = 13;    // Input4 conectada al pin D7
int ENB = 14;    // ENB conectada al pin D5

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  myservo.write(90);
  pinMode (ENB, OUTPUT); 
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  //Preparamos la salida para que el motor gire en un sentido
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);

  delay(3000);
}

void loop() {
  for (pos = 75; pos <= 103; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    analogWrite(ENB,(pos*3));
    delay(60);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 103; pos >= 75; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    analogWrite(ENB,(pos*3));
    delay(60);                       // waits 15ms for the servo to reach the position
  }
}

