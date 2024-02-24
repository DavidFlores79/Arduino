#include <Arduino.h>
#include <L298N.h>

// Definir pines para los motores y encoders
const unsigned int IN1 = 6;
const unsigned int IN2 = 7;
const unsigned int EN = 11;
const unsigned int IN3 = 9;
const unsigned int IN4 = 8;
const unsigned int EN1 = 10;
#define encoderRight 3
#define encoderLeft 2

// Definir resolución de encoders y dimensiones del robot
const float encoderResolution = 20.0;
const float wheelDiameter_mm = 67.2;
const float wheelBase = 130;
const float maxEncoderVelocity = 100.0;  // Ajusta según sea necesario

// Variables para el control PID
float kp_linear = 1.0;  // Ganancia proporcional para control PID lineal
float ki_linear = 0.0;  // Ganancia integral para control PID lineal
float kd_linear = 0.01;  // Ganancia derivativa para control PID lineal

float kp_angular = 1.0;  // Ganancia proporcional para control PID angular
float ki_angular = 0.0;  // Ganancia integral para control PID angular
float kd_angular = 0.01;  // Ganancia derivativa para control PID angular

// Variables para la navegación
float targetX = 2.0;  // Coordenada x deseada en cm
float targetY = 3.0;  // Coordenada y deseada en cm

// Variables para el control de velocidad
float targetLinearVelocity = 0.0;   // Velocidad lineal deseada en m/s
float targetAngularVelocity = 0.0;  // Velocidad angular deseada en rad/s

// Variables para el control de motores
volatile long encoderRightCount = 0;
volatile long encoderLeftCount = 0;

L298N motorLeft(EN, IN1, IN2);
L298N motorRight(EN1, IN3, IN4);

// Variables globales para la odometría
volatile long prevEncoderRightCount = 0;
volatile long prevEncoderLeftCount = 0;
unsigned long prevTime = 0;
float currentXValue = 0.0;
float currentYValue = 0.0;

//Calcular las velocidades actuales
unsigned long speedPrevTime = 0;  // Variable para almacenar el tiempo previo
long speedPrevTicksRight = 0;     // Variable para almacenar los ticks previos de la rueda derecha
long speedPrevTicksLeft = 0;      // Variable para almacenar los ticks previos de la rueda izquierda


void setup() {
  Serial.begin(9600);

  pinMode(encoderRight, INPUT);
  pinMode(encoderLeft, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderRight), encoderInterruptA, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderLeft), encoderInterruptB, RISING);
}

void loop() {
  // Calcular las velocidades lineales y angulares deseadas
  calculateVelocities();

  // Aplicar control PID para ajustar las velocidades
  // applyPID();

  // Controlar los motores con las velocidades ajustadas
  controlMotors();

  // Actualizar las posiciones actuales
  updatePosition();

  // Resto del código...
}

void calculateVelocities() {
  // Calcular la diferencia entre la posición actual y la deseada
  float deltaX = targetX - currentXPosition();
  float deltaY = targetY - currentYPosition();

  // Calcular la distancia y el ángulo al objetivo
  float distanceToTarget = sqrt(deltaX * deltaX + deltaY * deltaY);
  float angleToTarget = atan2(deltaY, deltaX);

  // Calcular las velocidades lineal y angular deseadas
  targetLinearVelocity = kp_linear * distanceToTarget;
  targetAngularVelocity = kp_angular * angleToTarget;

  if (distanceToTarget < 0.1) {  // Ajusta el umbral según sea necesario
    stopRobot();
    return;  // Salir del bucle si se alcanza el objetivo
  }
}

void applyPID() {
  // Control PID para la velocidad lineal
  static float integralLinear = 0.0;
  static float lastErrorLinear = 0.0;
  float errorLinear = targetLinearVelocity - currentLinearVelocity();
  integralLinear += errorLinear;
  float derivativeLinear = errorLinear - lastErrorLinear;

  // Ajuste de las ganancias PID para el control lineal
  float linearPWM = kp_linear * errorLinear + ki_linear * integralLinear + kd_linear * derivativeLinear;
  linearPWM = constrain(linearPWM, 0, 255);  // Limitar el valor para el control de velocidad

  // Control PID para la velocidad angular
  static float integralAngular = 0.0;
  static float lastErrorAngular = 0.0;
  float errorAngular = targetAngularVelocity - currentAngularVelocity();
  integralAngular += errorAngular;
  float derivativeAngular = errorAngular - lastErrorAngular;

  // Ajuste de las ganancias PID para el control angular
  float angularPWM = kp_angular * errorAngular + ki_angular * integralAngular + kd_angular * derivativeAngular;
  angularPWM = constrain(angularPWM, 0, 255);  // Limitar el valor para el control de velocidad

  // Aplicar las correcciones PID a las velocidades deseadas
  targetLinearVelocity += linearPWM;
  targetAngularVelocity += angularPWM;

  // Actualizar los errores anteriores
  lastErrorLinear = errorLinear;
  lastErrorAngular = errorAngular;
}


void controlMotors() {

  // Convertir velocidades de rueda a velocidades de encoder
  float leftWheelVelocity = targetLinearVelocity + (targetAngularVelocity * wheelBase / 2);
  float rightWheelVelocity = targetLinearVelocity - (targetAngularVelocity * wheelBase / 2);

  float leftEncoderVelocity = leftWheelVelocity / (PI * (wheelDiameter_mm / 1000.0)) * encoderResolution;
  float rightEncoderVelocity = rightWheelVelocity / (PI * (wheelDiameter_mm / 1000.0)) * encoderResolution;

  // Calcular el error proporcional
  float leftError = leftEncoderVelocity - currentXPosition();
  float rightError = rightEncoderVelocity - currentYPosition();

  // Aplicar control proporcional
  int leftPWM = map(abs(leftError), 0, maxEncoderVelocity, 0, maxEncoderVelocity);
  int rightPWM = map(abs(rightError), 0, maxEncoderVelocity, 0, maxEncoderVelocity);

  // Limitar el valor del PWM
  leftPWM = constrain(leftPWM, 0, maxEncoderVelocity);
  rightPWM = constrain(rightPWM, 0, maxEncoderVelocity);

  // Controlar los motores usando la biblioteca L298N
  motorLeft.setSpeed(leftPWM);
  motorRight.setSpeed(rightPWM);

  // Establecer la dirección de los motores
  if (leftEncoderVelocity > 0) {
    motorLeft.forward();
  } else {
    motorLeft.stop();
  }

  if (rightEncoderVelocity > 0) {
    motorRight.forward();
  } else {
    motorRight.stop();
  }


  // Imprimir información en el puerto serie
  Serial.print("X: ");
  Serial.print(currentXPosition());
  Serial.print(" Y: ");
  Serial.print(currentYPosition());
  Serial.print(" Distancia: ");
  Serial.print(sqrt((targetX - currentXPosition()) * (targetX - currentXPosition()) + (targetY - currentYPosition()) * (targetY - currentYPosition())));
  Serial.print(" Vel_Lineal: ");
  Serial.print(targetLinearVelocity);
  Serial.print(" Vel_Angular: ");
  Serial.print(targetAngularVelocity);
  Serial.print(" leftPWM: ");
  Serial.print(leftPWM);
  Serial.print(" rightPWM: ");
  Serial.println(rightPWM);
}

void stopRobot() {
  targetLinearVelocity = 0.0;
  targetAngularVelocity = 0.0;
  controlMotors();  // Detener los motores
}

void updatePosition() {
  // Obtener la diferencia en el conteo de encoders
  long deltaRight = encoderRightCount - prevEncoderRightCount;
  long deltaLeft = encoderLeftCount - prevEncoderLeftCount;

  // Calcular la distancia recorrida por cada rueda
  float distanceRight = deltaRight / encoderResolution * PI * wheelDiameter_mm / 1000.0;
  float distanceLeft = deltaLeft / encoderResolution * PI * wheelDiameter_mm / 1000.0;

  // Calcular la distancia total recorrida
  float distance = (distanceRight + distanceLeft) / 2.0;

  // Calcular el cambio en las posiciones X e Y
  float deltaX = distance * cos(targetAngularVelocity);
  float deltaY = distance * sin(targetAngularVelocity);

  // Actualizar las posiciones actuales
  currentXValue += deltaX;
  currentYValue += deltaY;

  // Actualizar los valores previos
  prevEncoderRightCount = encoderRightCount;
  prevEncoderLeftCount = encoderLeftCount;
}

float currentXPosition() {
  return currentXValue;
}

float currentYPosition() {
  return currentYValue;
}

float currentLinearVelocity() {
  // Utiliza la función calculateWheelSpeed para obtener la velocidad lineal actual
  return calculateWheelSpeed(encoderLeftCount, prevEncoderLeftCount) + calculateWheelSpeed(encoderRightCount, prevEncoderRightCount);
}

float currentAngularVelocity() {
  // Utiliza la función calculateWheelSpeed para obtener la velocidad angular actual
  float leftSpeed = calculateWheelSpeed(encoderLeftCount, prevEncoderLeftCount);
  float rightSpeed = calculateWheelSpeed(encoderRightCount, prevEncoderRightCount);

  // Calcula la velocidad angular utilizando la diferencia entre las velocidades de las ruedas
  float angularVelocity = (rightSpeed - leftSpeed) / wheelBase;

  return angularVelocity;
}

float calculateWheelSpeed(volatile long currentTicks, volatile long &speedPrevTicks) {
  float ticksPerRevolution = encoderResolution * 2 * PI;   // Ticks por revolución en radianes
  float distancePerRevolution_mm = wheelDiameter_mm * PI;  // Circunferencia de la rueda en milímetros

  // Calcular el tiempo transcurrido desde la última lectura
  unsigned long currentTime = micros();
  float elapsedTime = (currentTime - speedPrevTime) / 1000000.0;  // Convertir a segundos

  // Calcular la diferencia en ticks desde la última lectura
  long ticksDifference = currentTicks - speedPrevTicks;

  // Calcular velocidad en milímetros por segundo
  float speed = (ticksDifference / ticksPerRevolution) * distancePerRevolution_mm / elapsedTime;

  // Actualizar el tiempo previo y los ticks previos
  speedPrevTime = currentTime;
  speedPrevTicks = currentTicks;

  return speed;
}

void encoderInterruptA() {
  encoderRightCount++;
}

void encoderInterruptB() {
  encoderLeftCount++;
}