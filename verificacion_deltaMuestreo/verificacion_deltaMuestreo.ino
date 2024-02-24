volatile unsigned muestreoActual = 0;
volatile unsigned muestreoAnterior = 0;
volatile unsigned deltaMuestreo = 0;

int k= 100;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  muestreoActual = millis();

  deltaMuestreo = (double) muestreoActual - muestreoAnterior;
  
  if ((double) muestreoActual - muestreoAnterior >= k){
    Serial.println(millis());
    muestreoAnterior = muestreoActual;
  }
  
}
