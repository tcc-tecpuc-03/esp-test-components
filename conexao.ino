#include "ESP8266WiFi.h"


const char* ssid = "Nome_da_rede_wifi";
const char* password = "Senha";

void setup(){

 Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

  }

  Serial.println("");
  Serial.println("wifi conectado");
    
  Serial.print("endereço ip: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

}

void loop(){

}