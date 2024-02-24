#include "EmonLib.h"

#define RatioTC 2000
#define ResCarga 91
#define VoltajeLinea 117

EnergyMonitor emon1;

float effective_value; //effective current
int current_sensor_pin = A0;// A0=GPIO17 Analog input pin that sensor is attached to
int device_on_current_threshold = 8; //if the current value (milliamps) is greater than this amount then the device is on
int deviceState;

/**
 * Configuración. Sólo se ejecuta la primera vez que se inicia el dispositivo
 */
void setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println("");
    Serial.println("");
    Serial.println("Lector de consumo eléctrico");

    // Indicamos dónde está conectado el sensor de corriente (pin analógico A0) y el factor de calibración.
    emon1.current(A0, RatioTC / ResCarga);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);

}

/**
 * Bucle infinito que se ejecuta constantemente
 */
void loop() {
  
    // Calculamos la corriente efectiva indicando el número de muestras a tomar de la entrada analógica.
    //double Irms = emon1.calcIrms(2480);
/*
    Serial.print("I = ");
    Serial.print(Irms);
    Serial.print(" A, P = ");

    // Potencia efectiva = corriente efectiva * voltaje de línea
    Serial.print(Irms * VoltajeLinea);
    Serial.println(" W");
*/

  
  currentMeter();
  if ( (deviceState == 1) && (digitalRead(LED_BUILTIN) == 1) ){
    Serial.println(" Caso 1");
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  }
  if ( (deviceState == 0) && (digitalRead(LED_BUILTIN) == 1) ){
    Serial.println(" Caso 2 - Encendido");
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  }
  delay(1000);                       // wait for a second
  
  currentMeter();
  if ( (deviceState == 1) && (digitalRead(LED_BUILTIN) == 0) ){
    Serial.println(" Caso 3");
    digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
  }
  if ( (deviceState == 0) && (digitalRead(LED_BUILTIN) == 0) ){
    Serial.println(" Caso 4 - Encendido");
    digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
  }
  delay(1000);                       // wait for a second
}

void currentMeter()
{
    double Irms = emon1.calcIrms(2480);
    effective_value = Irms * VoltajeLinea;
      if(effective_value > device_on_current_threshold){
        deviceState = 1 ;
      }else{
        deviceState = 0 ;
      }
}

