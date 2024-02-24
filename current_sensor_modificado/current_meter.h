extern int current_sensor_pin;
float effective_value; //effective current
extern int deviceState;
extern int device_on_current_threshold;

void pins_init()
{
pinMode(current_sensor_pin, INPUT);
}

uint32_t start_time = millis();

void currentMeter()
{
    // Calculamos la corriente efectiva indicando el número de muestras a tomar de la entrada analógica.
    double Irms = emon1.calcIrms(2480);

    Serial.print("I = ");
    Serial.print(Irms);
    Serial.print(" A, P = ");

    // Potencia efectiva = corriente efectiva * voltaje de línea
    Serial.print(Irms * VoltajeLinea);
    Serial.println(" W");
    effective_value = Irms * VoltajeLinea;

      if(effective_value > device_on_current_threshold){
        Serial.println("Switch is on");
        deviceState = 1 ;
      }else{
        Serial.println("Switch is off");
        deviceState = 0 ;
      }
}
