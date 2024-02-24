volatile unsigned muestreoActualInterrupcion = 0;
volatile unsigned muestreoAnteriorInterrupcion = 0;
volatile unsigned deltaMuestreoInterrupcion = 0;
const int encoderR = 3;
int i=0;

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(encoderR), deltaTick, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(encoderR), deltaTick, FALLING);
  Serial.begin(9600);
}

void deltaTick(){
  deltaMuestreoInterrupcion = muestreoActualInterrupcion - muestreoAnteriorInterrupcion;
  muestreoAnteriorInterrupcion = millis();
  i++;
    
}

void loop() {
  // put your main code here, to run repeatedly:
  muestreoActualInterrupcion = millis();
  Serial.print(i);
  Serial.print(" - ");
  Serial.println(deltaMuestreoInterrupcion);
}
