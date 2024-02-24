#include <AFMotor.h>

AF_DCMotor motorDerecho(4);
AF_DCMotor motorIzquierdo(1);

volatile unsigned muestreoActualInterrupcionR = 0;        // variables para definiciòn del tiempo de interrupciòn y calculo de la velocidad motor derecho
volatile unsigned muestreoAnteriorInterrupcionR = 0;
volatile unsigned deltaMuestreoInterrupcionR = 0;
int encoderR = 3;   // pin de conexiòn del encoder derecho

volatile unsigned muestreoActualInterrupcionL = 0;        // variables para definiciòn del tiempo de interrupciòn y calculo de la velocidad motor derecho
volatile unsigned muestreoAnteriorInterrupcionL = 0;
volatile unsigned deltaMuestreoInterrupcionL = 0;
int encoderL = 2;   // pin de conexiòn del encoder derecho


double frecuenciaR = 0;                                  // frecuencia de interrupciòn llanta R
double Wr = 0;                                           // Velocidad angular R
double Vr = 0;                                           // velocidad Lineal
double frecuenciaL = 0;                                  // frecuencia de interrupciòn llanta R
double Wl = 0;                                           // Velocidad angular R
double Vl = 0;                                           // velocidad Lineal


int N = 20;                                              // nùmero de ranuras del encoder
float diametro = 6.97;                                    // diametro de la llanta cm


void setup() {
  attachInterrupt(digitalPinToInterrupt(encoderR),REncoder,RISING);                // linea para añadir una interrupciòn a un PIN
  attachInterrupt(digitalPinToInterrupt(encoderL),LEncoder,RISING);                // linea para añadir una interrupciòn a un PIN

  Serial.begin(9600);                                                               // inicio de la comunicaciòn serial

  // dar de alta la velocidad de los motores en 0:
  motorDerecho.setSpeed(0);
  motorDerecho.run(RELEASE);
  motorIzquierdo.setSpeed(0);
  motorIzquierdo.run(RELEASE);
  delay(2000);

}

void REncoder() {                                                                                    // funciòn de interrupciòn del enconder llanta derecha
      deltaMuestreoInterrupcionR = muestreoActualInterrupcionR -  muestreoAnteriorInterrupcionR;     // diferencia tiempos de interruciones de ticks del motor     
      muestreoAnteriorInterrupcionR = muestreoActualInterrupcionR;                                   // se actualiza el tiempo de interrupciòn anterior
      frecuenciaR = (1000)/(double) deltaMuestreoInterrupcionR;                                       // frecuencia de interrupciòn 
      Wr = ((2*3.1415926535897932384626433832795)/N)*frecuenciaR;                                                              // frecuencia angular Rad/s
      Vr= Wr*(diametro/2);                                                                            // velocidad lineal cm/s
 } 

void LEncoder() {                                                                                    // funciòn de interrupciòn del enconder llanta derecha
      deltaMuestreoInterrupcionL = muestreoActualInterrupcionL -  muestreoAnteriorInterrupcionL;     // diferencia tiempos de interruciones de ticks del motor     
      muestreoAnteriorInterrupcionL = muestreoActualInterrupcionL;                                   // se actualiza el tiempo de interrupciòn anterior
      frecuenciaL = (1000)/(double) deltaMuestreoInterrupcionL;                                       // frecuencia de interrupciòn 
      Wl = ((2*3.1415926535897932384626433832795)/N)*frecuenciaL;                                                              // frecuencia angular Rad/s
      Vl= Wl*(diametro/2);                                                                            // velocidad lineal cm/s
 } 

void loop() { 
    muestreoActualInterrupcionR = millis();                     // se asigna el tiempo de ejecuciòn a el muestreo actual
    muestreoActualInterrupcionL = millis();                     // se asigna el tiempo de ejecuciòn a el muestreo actual
    Serial.print(Vr);                                         // se muestra el tiempo entre TIC y TIC
    Serial.print(" ");
    Serial.println(Vl);                                         // se muestra el tiempo entre TIC y TIC
    motorDerecho.setSpeed(0);
    motorDerecho.run(FORWARD);
    motorIzquierdo.setSpeed(0);
    motorIzquierdo.run(FORWARD);
    delayMicroseconds(10);
}
