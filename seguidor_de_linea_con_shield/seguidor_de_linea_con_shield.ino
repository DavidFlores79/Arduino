// 
// Aqui se escribe la libreria necesaria para manejar los motores con el shield
#include <AFMotor.h>

//se dan de alta los motores
AF_DCMotor motor(1);
AF_DCMotor motor1(3);

//se dan de alta los sensores
int sensorPin = 14;
int sensorValue;
int sensorPin2 = 15;
int sensorValue2;

void setup() 
{
  Serial.begin(9600); // Abrimos comunicación Serial

  // definimos la velocidad inicial y se liberan 
  //los motores para evitar que el carro se mueva
  motor.setSpeed(50);
  motor.run(RELEASE);
  motor1.setSpeed(50);
  motor1.run(RELEASE);

  // los sensores opticos se declaran como salidas
  pinMode(sensorPin,INPUT);
  pinMode(sensorPin2,INPUT);

  
} 


void loop()
{
  // se leen los dos sensores y se imprimen los resultados
  sensorValue = digitalRead(sensorPin);
  Serial.print("Valor 1: ");
  Serial.print(sensorValue);
  sensorValue2 = digitalRead(sensorPin2);
  Serial.print(" Valor 2: ");
  Serial.println(sensorValue2);
  
  // se establece que los motores iran hacia adelante
    motor.run(FORWARD);
    motor1.run(FORWARD);


// si el sensor derecho marca fuera de linea puede moverse a la izq y viceversa
    if (sensorDer == 1){
            motor.setSpeed(100);
    }else{
            motor.setSpeed(70);
    }
    
    if (sensorIzq == 1){
            motor.setSpeed(70);
    }else{
            motor.setSpeed(100);
    }
delay (50);  
}
