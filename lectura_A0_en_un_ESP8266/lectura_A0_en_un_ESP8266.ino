int current_sensor_pin = A0;// A0=GPIO17 Analog input pin that sensor is attached to

void setup() {
  //Iniciamos la Comunicacion Serial a 9600 baudios
  Serial.begin(115200);
}

void loop() {
  int sensorValue = analogRead(current_sensor_pin); //Lectura del ADC 
  float voltage = sensorValue * (3.3 / 1023.0); //escalamos a voltaje 

  
 //Enviamos por el puerto serie
  Serial.print("ADC= ");
  Serial.print(sensorValue);
  Serial.print("  Voltaje= ");
  Serial.println(voltage); 
  
  delay(100);  
}
