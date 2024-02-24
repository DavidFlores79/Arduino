#include "EmonLib.h"

#define RatioTC 2000
#define ResCarga 91
#define VoltajeLinea 117

EnergyMonitor emon1;

float effective_value; //effective current
int current_sensor_pin = A0;// A0=GPIO17 Analog input pin that sensor is attached to
int device_on_current_threshold = 8; //if the current value (milliamps) is greater than this amount then the device is on
int deviceState = 0;


//para las notificaciones directas cada 5 segundos
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 500;           // interval at which to blink (milliseconds)

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

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
        // save the last time you blinked the LED
        currentMeter();
        Serial.println(deviceState);
        previousMillis = currentMillis;        
  }

}
