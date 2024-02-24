#include <Arduino.h>
#include <L298N.h>

// Definir pines para los motores
const unsigned int IN1 = 6;
const unsigned int IN2 = 7;
const unsigned int EN = 11;

const unsigned int IN3 = 8;
const unsigned int IN4 = 9;
const unsigned int EN1 = 10;

// Crear instancias de motores
L298N motorA(EN, IN1, IN2);
L298N motorB(EN1, IN3, IN4);

// Definir pines para los encoders
#define encoderPin1 2
#define encoderPin2 3

// Definir resolución de encoders y dimensiones del robot
const float encoderResolution = 20.0;  // Pulsos por revolución del encoder
const float wheelDiameter = 0.067;        // Diámetro de la rueda en metros
const float wheelBase = 0.0107;            // Distancia entre ruedas en metros


// Variables para el control de velocidad
float targetLinearVelocity = 0.008;  // Velocidad lineal deseada en m/s
float targetAngularVelocity = 0.005; // Velocidad angular deseada en rad/s

// Variables para el control de motores
volatile long encoderCount1 = 0;
volatile long encoderCount2 = 0;

void setup() {
  Serial.begin(9600);

  // Configurar pines de encoder como entradas y habilitar las interrupciones
  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoderPin1), encoderInterrupt1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin2), encoderInterrupt2, CHANGE);
}

void loop() {
  // Calcular velocidades de cada rueda usando seno y coseno
  float leftWheelVelocity = targetLinearVelocity - (targetAngularVelocity * wheelBase / 2);
  float rightWheelVelocity = targetLinearVelocity + (targetAngularVelocity * wheelBase / 2);

  // Convertir velocidades de rueda a velocidades de encoder (pulsos por segundo)
  float leftEncoderVelocity = leftWheelVelocity / (PI * wheelDiameter) * encoderResolution;
  float rightEncoderVelocity = rightWheelVelocity / (PI * wheelDiameter) * encoderResolution;

  // Controlar los motores con las velocidades calculadas
  controlMotors(leftEncoderVelocity, rightEncoderVelocity);
}

void controlMotors(float leftSpeed, float rightSpeed) {
  // Aplicar control proporcional para ajustar la velocidad de los motores
  int leftPWM = constrain(leftSpeed * 70, -70, 70);
  int rightPWM = constrain(rightSpeed * 70, -70, 70);

  // Controlar los motores usando la biblioteca L298N
  motorA.setSpeed(abs(leftPWM));
  motorB.setSpeed(abs(rightPWM));

  if (leftPWM > 0) {
    motorA.forward();
  } else {
    motorA.backward();
  }

  if (rightPWM > 0) {
    motorB.forward();
  } else {
    motorB.backward();
  }
}

void encoderInterrupt1() {
  encoderCount1++;
}

void encoderInterrupt2() {
  encoderCount2++;
}
