#include "EmonLib.h"

#define RatioTC 2000
#define ResCarga 91
#define VoltajeLinea 117

EnergyMonitor emon1;
float effective_value; //effective current
int current_sensor_pin = A0;// A0=GPIO17 Analog input pin that sensor is attached to
int device_on_current_threshold = 17; //if the current value (milliamps) is greater than this amount then the device is on
bool deviceState = 0;        // current state of the switch
int lastDeviceState = 0;    // previous state of the switch

//para las notificaciones directas cada medio segundo
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 500;           // interval at which to blink (milliseconds)

void setup()
{  
  Serial.begin(115200);
  
  // Indicamos dÃ³nde estÃ¡ conectado el sensor de corriente (pin analÃ³gico A0) y el factor de calibraciÃ³n.
  emon1.current(current_sensor_pin, RatioTC / ResCarga);
  //double Irms = emon1.calcIrms(2480);
}

void loop()
{
  currentMeter();
  
  Serial.print(effective_value);	       // Apparent power
  Serial.print(" **** ");
  Serial.println(deviceState);		       // Irms
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

