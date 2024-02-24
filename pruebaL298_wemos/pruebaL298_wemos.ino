/* 
 Ejemplo de control de motor DC usando modulo L298
 http://electronilab.co/tienda/driver-dual-para-motores-full-bridge-l298n/
 
 Creado 16/05/14
 por Andres Cruz
 ELECTRONILAB.CO
 */

int IN3 = 12;    // Input3 conectada al pin D6
int IN4 = 13;    // Input4 conectada al pin D7
int ENB = 14;    // ENB conectada al pin D5


void setup()
{
 pinMode (ENB, OUTPUT); 
 pinMode (IN3, OUTPUT);
 pinMode (IN4, OUTPUT);
}
void loop()
{
  //Preparamos la salida para que el motor gire en un sentido
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);
  // Aplicamos PWM al pin ENB, haciendo girar el motor, cada 2 seg aumenta la velocidad
  analogWrite(ENB,300);
  delay(2000);
  analogWrite(ENB,430);
  delay(2000);
  analogWrite(ENB,550);
  delay(2000);
  // Apagamos el motor y esperamos 5 seg
  analogWrite(ENB,0);
  delay(5000);
}
