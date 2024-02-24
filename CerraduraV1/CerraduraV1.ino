#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
//OLED
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);
//SERVIDOR
#define SERVER_IP "sensores.enlacetecnologias.mx/api/login"
WiFiMulti wifiMulti;
const uint32_t connectTimeoutMs = 5000;
//ENTRADAS Y SALIDAS
#define RELAY_PIN 2    // PIN LED O MOTOR
#define BUTTON_PIN 25  // PIN BOTON SALIDA
#define LED_ROJO 26
#define LED_VERDE 27
//DATOS RFID
#define SS_PIN 13  //PIN SS RFID
#define RST_PIN 4  // PIN RST RFID
MFRC522 rfid(SS_PIN, RST_PIN);
String Uid;
String Mnsj;
//DEBOUNCE
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin



void setup() {
  //RFID
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  ImprimirOled();
  display.print("Pase su tarjeta ");  //texto a mostrar
  display.display();
  inicializandoWIFI();
  //ConexionWIFI();
  SPI.begin(18, 19, 23, 13);   //SCK, MISO, MOSI, SS
  rfid.PCD_Init();             // init MFRC522
  pinMode(RELAY_PIN, OUTPUT);  // initialize pin as an output.
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // lock the door
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Pase la tarjeta");

  //KEYPAD
}

void loop() {
  GetUID();  //Lectura para comparar con pagina
  AperturaManual();
}

void GetUID() {
  //   if ( ! rfid.PICC_IsNewCardPresent()) {
  //     return;
  //  }

  //  // Select one of the cards
  //  if ( ! rfid.PICC_ReadCardSerial()) {
  //     return;
  //  }

  //  char str[32] = "";
  //  array_to_string(rfid.uid.uidByte, 4, str); //Insert (byte array, length, char array for output)
  //  Serial.println(str); //Print the output uid string
  Uid = "5AC508B1";
  ConexionWIFI();
  //  rfid.PICC_HaltA();
}

void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
  }
  buffer[len * 2] = '\0';
}

void ConexionWIFI() {
  // wait for WiFi connection
  //if ((WiFi.status() == WL_CONNECTED)) {
  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {

    imprimirConexion();

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP);  //HTTP
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");

    //String uid = getUID();
    //Serial.println(Uid);
    Uid = "538220AA";
    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"uid\":\"" + Uid + "\"}");
    Serial.println(httpCode);
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      //Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        DynamicJsonDocument doc(1024);
        String myJson = http.getString();
        int respuesta = http.GET();
        deserializeJson(doc, myJson);
        JsonObject responseJson = doc.as<JsonObject>();
        String status = responseJson["status"];
        int code = responseJson["code"];
        String name = responseJson["user"]["name"];
        String message = responseJson["message"];
        String hora_salida = responseJson["user"]["horario_salida"];
        String hora_entrada = responseJson["user"]["horario_entrada"];

        //imprimir el JSON del response
        //*for (JsonPair kv : responseJson) {
        // if(kv.value().as<String>() == "DHT11") {
        // Serial.println(kv.key().c_str());
        // Serial.println(kv.value().as<String>());
        // }
        //}
        Serial.println("Bienvenido " + name);

      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        // Serial.println("ACCESO DENEGADO");
        switch (httpCode) {
          case 400:
            Serial.println("Acceso denegado");
            break;
          case 401:
            Serial.println("Usuario Invalido");
            break;
          default:

            break;
        }
      }
    }


    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
  delay(10000);
}

void inicializandoWIFI() {

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("MIHOGAR", "e256262024");
  wifiMulti.addAP("FamFlores", "e256262024");
  wifiMulti.addAP("ElPatron", "e256262024");
}

void imprimirConexion() {
  Serial.print("WiFi connected: ");
  Serial.print(WiFi.SSID());
  Serial.print(" ");
  Serial.println(WiFi.localIP());
}

void ActivacionCerradura() {
  digitalWrite(RELAY_PIN, HIGH);  //manda señal de activacion al 2n2222
  display.clearDisplay();
  display.println("Abriendo cerradura...");
  display.display();
  digitalWrite(LED_VERDE, HIGH);
  delay(5000);                   //lo mantiene abierto por 3 segundos
  digitalWrite(RELAY_PIN, LOW);  //lo cierra
  digitalWrite(LED_VERDE, LOW);
  display.clearDisplay();
}

void AperturaManual() {

  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW) {
    Serial.println("The button is pressed");
    display.clearDisplay();
    display.print("Abriendo cerradura...");
    display.display();
    digitalWrite(LED_VERDE, HIGH);  //ENCIENDE LED VERDE
    digitalWrite(RELAY_PIN, HIGH);  //ENCIENDE RELAY
    delay(5000);
    digitalWrite(LED_VERDE, LOW);  //APAGA LED
    digitalWrite(RELAY_PIN, LOW);  //APAGA RELAY
    display.clearDisplay();

  } else if (lastState == LOW && currentState == HIGH) {
  }
  // save the the last state
  lastState = currentState;
}

void ImprimirOled() {
  display.clearDisplay();
  display.setTextSize(1);               //tamaño del texto
  display.setTextColor(SSD1306_WHITE);  //color del texto
  display.setCursor(2, 5);              //Ubicacion del texto
}