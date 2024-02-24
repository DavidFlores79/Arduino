int sensorPin = 14;
int sensorValue;
int sensorPin2 = 15;
int sensorValue2;

void setup() {
  // put your setup code here, to run once:
  pinMode(sensorPin,INPUT);
  pinMode(sensorPin2,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.begin(9600);
  sensorValue = digitalRead(sensorPin);
  Serial.print("Valor 1: ");
  Serial.print(sensorValue);
  sensorValue2 = digitalRead(sensorPin2);
  Serial.print(" Valor 2: ");
  Serial.println(sensorValue2);

  delay(1000);
}
