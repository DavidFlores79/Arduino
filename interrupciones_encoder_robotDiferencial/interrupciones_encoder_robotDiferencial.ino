
const int encoderR = 2;
int i=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(encoderR), tick, FALLING);
}

void tick(){
  i++;
  Serial.println(i);
}

void loop() {
  // put your main code here, to run repeatedly:

}
