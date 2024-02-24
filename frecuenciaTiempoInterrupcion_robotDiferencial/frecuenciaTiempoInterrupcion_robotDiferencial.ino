#include <AFMotor.h>

AF_DCMotor motorDerecho(4);
AF_DCMotor motorIzquierdo(1);

volatile unsigned muestreoActualInterrupcionR = 0;        // variables para definiciòn del tiempo de interrupciòn y calculo de la velocidad motor derecho
volatile unsigned muestreoAnteriorInterrupcionR = 0;
volatile unsigned deltaMuestreoInterrupcionR = 0;
int i=0;

int encoderR = 3;   // pin de conexiòn del encoder derecho

void setup() {
  attachInterrupt(1,REncoder,FALLING);                // linea para añadir una interrupciòn a un PIN
  Serial.begin(9600);                                                               // inicio de la comunicaciòn serial

  // dar de alta la velocidad de los motores en 0:
  motorDerecho.setSpeed(0);
  motorDerecho.run(RELEASE);
  motorIzquierdo.setSpeed(0);
  motorIzquierdo.run(RELEASE);
  delay(2000);
  
}

void REncoder() {                                                                                    // funciòn de interrupciòn del enconder llanta derecha
      i++;
      deltaMuestreoInterrupcionR = muestreoActualInterrupcionR -  muestreoAnteriorInterrupcionR;     // diferencia tiempos de interruciones de ticks del motor     
      muestreoAnteriorInterrupcionR = muestreoActualInterrupcionR;                                   // se actualiza el tiempo de interrupciòn anterior
 } 

void loop() { 
    //Serial.print(i);
    //Serial.print(" - ");
    muestreoActualInterrupcionR = millis();                     // se asigna el tiempo de ejecuciòn a el muestreo actual
    Serial.println(deltaMuestreoInterrupcionR);                 // se muestra el tiempo entre TIC y TIC
    motorDerecho.setSpeed(130);
    motorDerecho.run(FORWARD);
}
