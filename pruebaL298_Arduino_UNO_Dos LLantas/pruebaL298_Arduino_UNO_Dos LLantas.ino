/* 
 Ejemplo de control de motor DC usando modulo L298
 http://electronilab.co/tienda/driver-dual-para-motores-full-bridge-l298n/
 
 Creado 16/05/14
 por Andres Cruz
 ELECTRONILAB.CO
 */

int IN1 = 6;  // Input3 conectada al pin 6
int IN2 = 7;  // Input4 conectada al pin 7
int IN3 = 8;  // Input3 conectada al pin 8
int IN4 = 9;  // Input4 conectada al pin 9

int ENA = 10;  // Input3 conectada al pin 10
int ENB = 11;  // Input4 conectada al pin 11

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(3000);
}
void loop() {
  //Preparamos la salida para que el motor gire en un sentido
  analogWrite(ENA, 50);
  analogWrite(ENB, 50);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(3000);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(3000);
  analogWrite(ENA, 70);
  analogWrite(ENB, 70);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(3000);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(3000);
}