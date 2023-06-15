#include "WifiClass.h"
#include <ESP8266WiFi.h>
#include <String.h>
#include "Arduino_JSON.h"

APIManager apiManager("DRIW_24Gh","17171407");

// rfid
#include "SPI.h"
#include "MFRC522.h"
#define SS_PIN D8
#define RST_PIN D0

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];

const int MAX_RFID_CARDS = 10; // Número máximo de cartões RFID a serem armazenados
String rfidCards[MAX_RFID_CARDS]; // Array de strings para armazenar os cartões RFID
int numRfidCards = 0; // Número atual de cartões RFID armazenados


void setup() {
  Serial.begin(9600);
  apiManager.begin();

  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println();
  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println();
  Serial.println(F("This code scan the MIFARE Classic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  rfidCards[0] ="A2 B3 D7 4C";
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] ||
      rfid.uid.uidByte[1] != nuidPICC[1] ||
      rfid.uid.uidByte[2] != nuidPICC[2] ||
      rfid.uid.uidByte[3] != nuidPICC[3]) {
    Serial.println(F("A new card has been detected."));

    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    // Armazena o cartão RFID no array de strings
    if (numRfidCards < MAX_RFID_CARDS) {
      String rfidCard;
      for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) {
          rfidCard += "0";
        }
        rfidCard += String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1) {
          rfidCard += " ";
        }
      }
      rfidCard.toUpperCase();
      rfidCards[numRfidCards] = rfidCard;      
      numRfidCards++;

      Serial.println("RFID card added to the array:");
      Serial.println(rfidCard);
    } else {
      Serial.println("Maximum number of RFID cards reached.");
    }
  } else {
    Serial.println(F("Card read previously."));
    printHex(rfid.uid.uidByte, rfid.uid.size);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Realiza a chamada de API com os cartões RFID armazenados
  String url = "http://wkf5h8-3303.csb.app/basket";
  String metodo = "POST";
  String dados = "{\"items\": [";

  for (int i = 0; i < numRfidCards; i++) {
    dados += "\"" + rfidCards[i] + "\"";
    if (i < numRfidCards - 1) {
      dados += ",";
    }
    Serial.print(dados);
  }

  dados += "]}";

  String resposta = apiManager.request(url, metodo, dados);
  Serial.println(resposta);

  delay(10000); // Aguarda 10 segundos antes de realizar outra chamada
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
