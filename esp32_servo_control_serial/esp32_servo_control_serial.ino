#include <ESP32Servo.h>
Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
int servoPin = 15;

void setup() {
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);           // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400);  // attaches the servo on pin 18 to the servo object
}

void loop() {
  while (Serial.available()) {
    String in_char = Serial.readStringUntil('\n');
    pos = in_char.toInt();
    Serial.println(pos);
    myservo.write(pos);
    delay(10);
  }
}