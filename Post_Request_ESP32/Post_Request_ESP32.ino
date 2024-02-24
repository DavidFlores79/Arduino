#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN    32  // ESP32 pin GIOP5 
#define RST_PIN   13 // ESP32 pin GIOP27 
#define RELAY_PIN 17 // ESP32 pin GIOP22 connects to relay

MFRC522 rfid(SS_PIN, RST_PIN);

byte keyTagUID[4] = {0x53, 0x82, 0x20, 0xAA};//valor llave 

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  pinMode(RELAY_PIN, OUTPUT); // initialize pin as an output.
  digitalWrite(RELAY_PIN, LOW); // lock the door
  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (rfid.uid.uidByte[0] == keyTagUID[0] &&
          rfid.uid.uidByte[1] == keyTagUID[1] &&
          rfid.uid.uidByte[2] == keyTagUID[2] &&
          rfid.uid.uidByte[3] == keyTagUID[3] ) {
        Serial.println("Access is granted");
        digitalWrite(RELAY_PIN, HIGH);  // unlock the door for 2 seconds
        delay(2000);
        digitalWrite(RELAY_PIN, LOW); // lock the door
      }
      else
      {
        Serial.print("Access denied, UID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}