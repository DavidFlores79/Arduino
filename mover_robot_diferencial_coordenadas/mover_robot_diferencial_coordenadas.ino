/*
Este programa utiliza la librería Encoder para leer los encoders conectados a los motores del robot y
calcular la distancia y ángulo necesarios para llegar a una coordenada específica (x,y). 
Luego, mueve el robot utilizando la función move() hasta que se alcance la distancia deseada.

Es importante tener en cuenta que para que este código funcione es necesario definir las constantes
WHEEL_RADIUS y ENCODER_RESOLUTION y MOTOR_SPEED con los valores correctos para tu robot y tambien 
asegurarte de que los pines de los encoders y los motores estan correctamente conectados.

La constante WHEEL_RADIUS representa el radio de las ruedas del robot en metros. Este valor se
utiliza para calcular la distancia recorrida por el robot a partir de la información del encoder.

La constante ENCODER_RESOLUTION representa la cantidad de pasos del encoder por revolución de la
rueda. Este valor se utiliza para calcular la distancia recorrida por el robot a partir de la 
información del encoder.

La constante MOTOR_SPEED representa la velocidad a la que se moverán los motores del robot en una escala
de 0 a 255. Este valor se utiliza para controlar la velocidad de los motores mientras el robot se mueve hacia
la coordenada específica.
*/

#include <Encoder.h> // librería para usar encoders

#define WHEEL_RADIUS 0.06
#define ENCODER_RESOLUTION 20
#define MOTOR_SPEED 70

Encoder encoderLeft(2, 3); // pines para el encoder izquierdo
Encoder encoderRight(4, 5); // pines para el encoder derecho

void setup() {
  pinMode(6, OUTPUT); // pin para el motor izquierdo
  pinMode(7, OUTPUT); // pin para el motor derecho
}

void moveToCoordinate(double x, double y) {
  // cálculo de distancia y ángulo para llegar a la coordenada (x,y)
  double distance = sqrt(x*x + y*y);
  double angle = atan2(y, x);

  // mover el robot a la distancia y ángulo calculados
  move(distance, angle);
}

void move(double distance, double angle) {
  // reiniciar los encoders
  encoderLeft.write(0);
  encoderRight.write(0);

  // calcular el número de pasos necesarios para alcanzar la distancia deseada
  int steps = (distance / (2 * 3.14 * WHEEL_RADIUS)) * ENCODER_RESOLUTION;

  // calcular la velocidad de los motores
  int speedLeft = cos(angle) * MOTOR_SPEED;
  int speedRight = sin(angle) * MOTOR_SPEED;

  // mover el robot hasta que se alcance el número de pasos
  while (encoderLeft.read() < steps && encoderRight.read() < steps) {
    analogWrite(6, speedLeft);
    analogWrite(7, speedRight);
  }

  // detener el robot
  analogWrite(6, 0);
  analogWrite(7, 0);
}
