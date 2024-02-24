// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>
#include <Servo.h> 
#include <NewPing.h>

//sensor pins
#define trig_pin A4 //analog input 1
#define echo_pin A5 //analog input 2

#define maximum_distance 200
boolean goesForward = false;
int distance = 100;

NewPing sonar(trig_pin, echo_pin, maximum_distance); //sensor function

AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// DC hobby servo
Servo servo_motor;
int j; 

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");
  servo_motor.attach(9);
  servo_motor.write(90);  
  // turn on motor
  motor3.setSpeed(210);
  motor3.run(RELEASE);
  motor4.setSpeed(220);
  motor4.run(RELEASE);

  delay(2000);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);
  distance = readPing();
  delay(100);


}

void loop() 
{ // INICIO DEL LOOP

  int distanceRight = 0;
  int distanceLeft = 0;
  delay(50);

  if (distance <= 20){

    moveStop();
    delay(300);
    moveBackward();
    delay(400);
    moveStop();
    delay(300);
    distanceRight = lookRight();
    delay(300);
    distanceLeft = lookLeft();
    delay(300);

    if (distanceRight >= distanceLeft){
      turnRight();
      delay(400);
      moveStop();
//      Serial.println("Giro DERECHA!!");
    }
    else{
      turnLeft();
      delay(400);
      moveStop();
//      Serial.println("Giro IZQUIERDA!!");
    }
  }
  else{
    moveForward();
//    moveStop(); 
//    Serial.println("Adelante!!");
 
  }
    distance = readPing();
    //Serial.println(distance);

} //FIN DEL LOOP


int lookRight(){  
  servo_motor.write(20);
  delay(500);
  int distance = readPing();
  //Serial.println(distance);
  delay(100);
  servo_motor.write(90);
  return distance;
}

int lookLeft(){
  servo_motor.write(180);
  delay(500);
  int distance = readPing();
  //Serial.println(distance);
  delay(100);
  servo_motor.write(90);
  return distance;
  delay(100);
}

int readPing(){
  delay(70);
  int cm = sonar.ping_cm();
  if (cm==0){
    cm=250;
  }
  return cm;
}

void moveStop(){
  
    motor3.run(RELEASE);
    motor4.run(RELEASE);
}

void moveForward(){

  if(!goesForward){

    goesForward=true;
    
    motor3.run(FORWARD);
    motor4.run(FORWARD);
  }
}

void moveBackward(){

  goesForward=false;

    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
/*
    delay(300);
  
    motor3.run(RELEASE);
    motor4.run(RELEASE);
*/
}

void turnRight(){

    motor3.run(BACKWARD);
    motor4.run(FORWARD);
    distance = readPing();
/*
    delay(400);
  
    motor3.run(RELEASE);
    motor4.run(RELEASE);
*/  
}

void turnLeft(){

    motor3.run(FORWARD);
    motor4.run(BACKWARD);
    distance = readPing();
/*
    delay(400);
  
    motor3.run(RELEASE);
    motor4.run(RELEASE);
*/    
}

