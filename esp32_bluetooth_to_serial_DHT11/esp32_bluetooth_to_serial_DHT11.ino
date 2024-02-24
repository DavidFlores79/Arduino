#include "BluetoothSerial.h"
#include "DHT.h"

#define DHTPIN 16       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 22  (AM2302), AM2321

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);

// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 2000;  // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(115200);
  dht.begin();
  SerialBT.begin("ESP32test");  //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  unsigned long currentMillis = millis();

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }

  if (currentMillis - previousMillis >= interval) {
    // save the last time
    previousMillis = currentMillis;

    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    String data = "t" + String(t) + "h" + String(h) + "\n";
    Serial.println(data);
    const char *mensaje = data.c_str();

    for (size_t i = 0; i < strlen(mensaje); i++) {
      SerialBT.write(mensaje[i]);
    }

    SerialBT.
  }

  delay(15);
}

















