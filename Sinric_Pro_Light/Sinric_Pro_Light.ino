// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProLight.h"

const char* ssid = "MIHOGAR";
const char* password = "e256262024";

#define WIFI_SSID         "MIHOGAR"    
#define WIFI_PASS         "e256262024"
#define APP_KEY           "6ec1abde-9dad-470b-b34e-b44fc3500d7b"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "443e24f7-f326-45d4-8718-6794fd7ab1d8-9e0cc4f5-2e18-4891-983b-2c79597704f6"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
#define LIGHT_ID          "5f1a7258e4c7460fd5b393a5"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BAUD_RATE         115200                // Change baudrate to your need
#define SOLID_RELE        LED_BUILTIN
#define TOUCH_SENSOR      D2

boolean sensorState = 0;         // current state of the button
boolean lightState = 0;
boolean lastSensorState = 0;     // previous state of the button
boolean myPowerState;


// define array of supported color temperatures
int colorTemperatureArray[] = {2200, 2700, 4000, 5500, 7000};  
int max_color_temperatures = sizeof(colorTemperatureArray) / sizeof(colorTemperatureArray[0]); // calculates how many elements are stored in colorTemperature array

// a map used to convert a given color temperature into color temperature index (used for colorTemperatureArray)
std::map<int, int> colorTemperatureIndex;

// initializes the map above with color temperatures and index values
// so that the map can be used to do a reverse search like
// int index = colorTemperateIndex[4000]; <- will result in index == 2
void setupColorTemperatureIndex() {
  Serial.printf("Setup color temperature lookup table\r\n");
  for (int i=0;i < max_color_temperatures; i++) {
    colorTemperatureIndex[colorTemperatureArray[i]] = i;
    Serial.printf("colorTemperatureIndex[%i] = %i\r\n", colorTemperatureArray[i], colorTemperatureIndex[colorTemperatureArray[i]]);
  }
}

// we use a struct to store all states and values for our light
struct {
  bool powerState = false;
  int brightness = 0;
  struct {
    byte r = 0;
    byte g = 0;
    byte b = 0;
  } color;
  int colorTemperature = colorTemperatureArray[0]; // set colorTemperature to first element in colorTemperatureArray array
} device_state; 

bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
  myPowerState = state;
  digitalWrite(SOLID_RELE, myPowerState?LOW:HIGH);
  device_state.powerState = state;
  return true; // request handled properly
}

bool onBrightness(const String &deviceId, int &brightness) {
  device_state.brightness = brightness;
  Serial.printf("Device %s brightness level changed to %d\r\n", deviceId.c_str(), brightness);
  return true;
}

bool onAdjustBrightness(const String &deviceId, int brightnessDelta) {
  device_state.brightness += brightnessDelta;
  Serial.printf("Device %s brightness level changed about %i to %d\r\n", deviceId.c_str(), brightnessDelta, device_state.brightness);
  brightnessDelta = device_state.brightness;
  return true;
}

bool onColor(const String &deviceId, byte &r, byte &g, byte &b) {
  device_state.color.r = r;
  device_state.color.g = g;
  device_state.color.b = b;
  Serial.printf("Device %s color changed to %d, %d, %d (RGB)\r\n", deviceId.c_str(), device_state.color.r, device_state.color.g, device_state.color.b);
  return true;
}

bool onColorTemperature(const String &deviceId, int &colorTemperature) {
  device_state.colorTemperature = colorTemperature;
  Serial.printf("Device %s color temperature changed to %d\r\n", deviceId.c_str(), device_state.colorTemperature);
  return true;
}

bool onIncreaseColorTemperature(const String &deviceId, int &colorTemperature) {
  int index = colorTemperatureIndex[device_state.colorTemperature];              // get index of stored colorTemperature
  index++;                                                                // do the increase
  if (index < 0) index = 0;                                               // make sure that index stays within array boundaries
  if (index > max_color_temperatures-1) index = max_color_temperatures-1; // make sure that index stays within array boundaries
  device_state.colorTemperature = colorTemperatureArray[index];                  // get the color temperature value
  Serial.printf("Device %s increased color temperature to %d\r\n", deviceId.c_str(), device_state.colorTemperature);
  colorTemperature = device_state.colorTemperature;                              // return current color temperature value
  return true;
}

bool onDecreaseColorTemperature(const String &deviceId, int &colorTemperature) {
  int index = colorTemperatureIndex[device_state.colorTemperature];              // get index of stored colorTemperature
  index--;                                                                // do the decrease
  if (index < 0) index = 0;                                               // make sure that index stays within array boundaries
  if (index > max_color_temperatures-1) index = max_color_temperatures-1; // make sure that index stays within array boundaries
  device_state.colorTemperature = colorTemperatureArray[index];                  // get the color temperature value
  Serial.printf("Device %s decreased color temperature to %d\r\n", deviceId.c_str(), device_state.colorTemperature);
  colorTemperature = device_state.colorTemperature;                              // return current color temperature value
  return true;
}

void getConected() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}


void setupSinricPro() {
  // get a new Light device from SinricPro
  SinricProLight &myLight = SinricPro[LIGHT_ID];

  // set callback function to device
  myLight.onPowerState(onPowerState);
  myLight.onBrightness(onBrightness);
  myLight.onAdjustBrightness(onAdjustBrightness);
  myLight.onColor(onColor);
  myLight.onColorTemperature(onColorTemperature);
  myLight.onIncreaseColorTemperature(onIncreaseColorTemperature);
  myLight.onDecreaseColorTemperature(onDecreaseColorTemperature);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  pinMode(SOLID_RELE, OUTPUT);
  pinMode(TOUCH_SENSOR, INPUT);
  getConected();

  if(WiFi.status()== WL_CONNECTED){
    setupColorTemperatureIndex(); // setup our helper map
    setupSinricPro();
  }else{
    Serial.println("WiFi Disconnected");
    getConected();
  }
}

void loop() {
  SinricPro.handle();
    sensorState = digitalRead(TOUCH_SENSOR);
    if (sensorState != lastSensorState) {
        lightState = digitalRead(SOLID_RELE);
        digitalWrite(SOLID_RELE, !lightState);
        myPowerState = lightState;
        // get Switch device back
        SinricProLight &myLight = SinricPro[LIGHT_ID];
        // send powerstate event
        myLight.sendPowerStateEvent(myPowerState); // send the new powerState to SinricPro server
        Serial.printf("Device %s turned %s (manually via touchSensor)\r\n", myLight.getDeviceId(), myPowerState?"on":"off");
    }
    // save the current state as the last state, for next time through the loop
    lastSensorState = sensorState;
}
