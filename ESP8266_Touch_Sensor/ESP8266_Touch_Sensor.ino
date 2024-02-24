#define TOUCH_SENSOR    D2
#define SOLID_RELE      LED_BUILTIN
#define BAUD_RATE       115200

boolean sensorState = 0;         // current state of the button
boolean lastSensorState = 0;     // previous state of the button

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  pinMode(TOUCH_SENSOR, INPUT);
  pinMode(SOLID_RELE, OUTPUT);
  
}

void loop() {
  sensorState = digitalRead(TOUCH_SENSOR);
  if (sensorState != lastSensorState) {
    if (sensorState == HIGH) {
      Serial.println("on");
      digitalWrite(SOLID_RELE, LOW);
    } else {
      Serial.println("off");
      digitalWrite(SOLID_RELE, HIGH);
    }
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastSensorState = sensorState;
}
