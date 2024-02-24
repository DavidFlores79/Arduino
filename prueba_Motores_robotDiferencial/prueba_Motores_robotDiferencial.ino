#include <AFMotor.h>

AF_DCMotor motorDerecho(1);
AF_DCMotor motorIzquierdo(4);

void setup() {
  // put your setup code here, to run once:
  motorDerecho.setSpeed(0);
  motorDerecho.run(RELEASE);
  motorIzquierdo.setSpeed(0);
  motorIzquierdo.run(RELEASE);
  delay(5000);

  motorDerecho.setSpeed(180);
  motorDerecho.run(FORWARD);
  motorIzquierdo.setSpeed(180);
  motorIzquierdo.run(FORWARD);
  delay(10000);

  motorDerecho.setSpeed(0);
  motorDerecho.run(RELEASE);
  motorIzquierdo.setSpeed(0);
  motorIzquierdo.run(RELEASE);

}

void loop() {
  // put your main code here, to run repeatedly:

}
