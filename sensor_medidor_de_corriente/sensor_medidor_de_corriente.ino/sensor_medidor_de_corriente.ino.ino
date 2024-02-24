// Definimos variables
float sensibilidad = 0.066;           // 5A = 185; 20A = 100   30A = 66
float ruido = 0.000;                  // filtro para eliminar ruido.
const int sensorIntensidad  = A0;         // Pin donde conectamos sensor.
float valorReposo = 2.50;
float intensidadPico = 0;
float tensionDeRed = 117.0;

void setup() {
  Serial.begin(115200);                //Iniciamos puerto serial.
}

void loop() {

  intensidadPico = leerCorriente();     // Leemos corriente.
  mostarValores();

}

float leerCorriente()
{
  float valorVoltajeSensor;
  float corriente = 0;
  long tiempo = millis();
  float intensidadMaxima = 0;
  float intensidadMinima = 0;
  while (millis() - tiempo < 500) //realizamos mediciones durante 0.5 segundos
  {
    valorVoltajeSensor = analogRead(sensorIntensidad) * (5.0 / 1023.0); //lectura del sensor en voltios
    corriente = 0.9 * corriente + 0.1 * ((valorVoltajeSensor - valorReposo) / sensibilidad); //Ecuación  para obtener la corriente
    if (corriente > intensidadMaxima)intensidadMaxima = corriente;
    if (corriente < intensidadMinima)intensidadMinima = corriente;
  }
  return (((intensidadMaxima - intensidadMinima) / 2) - ruido);
}

void mostarValores() {

  float Irms = intensidadPico * 0.707;            //Intensidad RMS = Ipico/(2^1/2)
  Serial.print("Intensidad Pico: ");
  Serial.print(intensidadPico, 3);
  Serial.print("A , Irms: ");
  Serial.print(Irms, 3);
  Serial.println("A.");
  delay(100);
}
