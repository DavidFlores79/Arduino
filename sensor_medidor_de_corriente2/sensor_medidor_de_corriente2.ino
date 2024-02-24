#include "EmonLib.h"

#define RatioTC 2000
#define ResCarga 91
#define VoltajeLinea 117

EnergyMonitor emon1;

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
}

/**
 * Bucle infinito que se ejecuta constantemente
 */
void loop() {
  
    // Calculamos la corriente efectiva indicando el número de muestras a tomar de la entrada analógica.
    double Irms = emon1.calcIrms(2480);

    Serial.print("I = ");
    Serial.print(Irms);
    Serial.print(" A, P = ");

    // Potencia efectiva = corriente efectiva * voltaje de línea
    Serial.print(Irms * VoltajeLinea);
    Serial.println(" W");


    //delay(1000);
}
