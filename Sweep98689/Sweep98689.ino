
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
Servo myservo2;
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo2.attach(10);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  myservo.write(90);

}
