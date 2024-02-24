int ledPin = 13;
int encoderR = 2;
//int encoderR = 3;
int estado = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(encoderR, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  estado = digitalRead(encoderR);
  if (estado == HIGH){
    digitalWrite(ledPin, HIGH);
    Serial.println(estado);
  }else{
    digitalWrite(ledPin, LOW);
    Serial.println(estado);
  }
}
