int in1 = 5;  // Pin que controla el sentido de giro Motor A
int in2 = 6;  // Pin que controla el sentido de giro Motor A

int in3 = 9;  // Pin que controla el sentido de giro Motor A
int in4 = 10;  // Pin que controla el sentido de giro Motor A

int vel = 50;

 
void setup ()
{
  pinMode(in1, OUTPUT);    // Configura  los pines como salida
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);    // Configura  los pines como salida
  pinMode(in4, OUTPUT);

  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

}
void loop()
{

  tomarLectura();
  
  digitalWrite(in1, LOW);  // GIRO DERECHA
  analogWrite(in2, vel);
  digitalWrite(in3, LOW);  // GIRO DERECHA
  analogWrite(in4, vel);

  delay(1000);
  
  digitalWrite(in1, LOW);  // PARA
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);  // PARA
  digitalWrite(in4, LOW);
  delay(1000);
  
  analogWrite(in1, vel);  // GIRO IZQUIERDA
  digitalWrite(in2, LOW);
  analogWrite(in3, vel);  // GIRO IZQUIERDA
  digitalWrite(in4, LOW);
  delay(1000);
  
  digitalWrite(in1, LOW);  // PARA
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);  // PARA
  digitalWrite(in4, LOW);
  delay(1000);
}

void tomarLectura() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  Serial.println(voltage);

  if(voltage < 3.5) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
