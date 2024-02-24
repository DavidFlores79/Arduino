/*
Author  : Andrea Lombardo
Site    : https://www.lombardoandrea.com
Source  : https://github.com/AndreaLombardo/L298N/

Here you can see how to work in a common configuration. 

Speed range go from 0 to 255, default is 100.
Use setSpeed(speed) to change.

Sometimes at lower speed motors seems not running.
It's normal, may depends by motor and power supply.

Wiring schema in file "L298N - Schema_with_EN_pin.png"
*/

// Include the library
#include <L298N.h>

// Pin definition
const unsigned int IN1 = 6;
const unsigned int IN2 = 7;
const unsigned int EN = 11;
// Pin definition
const unsigned int IN3 = 8;
const unsigned int IN4 = 9;
const unsigned int EN1 = 10;

// Create one motor instance
L298N motorA(EN, IN1, IN2);
L298N motorB(EN1, IN3, IN4);

void setup()
{
  // Used to display information
  Serial.begin(9600);

  // Wait for Serial Monitor to be opened
  while (!Serial)
  {
    //do nothing
  }

  // Set initial speed
  motorA.setSpeed(60);
  motorB.setSpeed(60);
}

void loop()
{

  // Tell the motor to go forward (may depend by your wiring)
  motorA.forward();
  motorB.forward();

  // Alternative method:
  // motor.run(L298N::FORWARD);

  //print the motor status in the serial monitor
  printSomeInfo();

  delay(3000);

  // Stop
  motorA.stop();
  motorB.stop();

  // Alternative method:
  // motor.run(L298N::STOP);

  printSomeInfo();

  // Change speed
  //motor.setSpeed(60);

  delay(3000);

  // Tell the motor to go back (may depend by your wiring)
  motorA.backward();

  // Alternative method:
  // motor.run(L298N::BACKWARD);

  printSomeInfo();

  motorA.setSpeed(60);

  delay(3000);

  // Stop
  motorA.stop();

  printSomeInfo();

  delay(3000);
}

/*
Print some informations in Serial Monitor
*/
void printSomeInfo()
{
  Serial.print("Motor is moving = ");
  Serial.print(motorA.isMoving());
  Serial.print(" at speed = ");
  Serial.println(motorA.getSpeed());
}