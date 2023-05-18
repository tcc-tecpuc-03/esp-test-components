#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include <ESP8266HTTPClient.h>
#include "Arduino_JSON.h"

const char* ssid = "Nome_da_rede_wifi";
const char* password = "Senha";

boolean res = true;
const char* host = "http://url/endpoints";


void setup() { 
  // Conexao com a internet
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("wifi conectado");
    
  Serial.print("Endereço ip: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

}

void loop(){
  
    if(res == true){

    //POST 
    // httpPOSTRequest(host);

    //GET

    //Recebendo em JSON    
    String dados = httpGETRequest(host);    
    Serial.print("dados: ");
    Serial.println(dados);
    Serial.println("");
    JSONVar objeto = JSON.parse(dados);
    
    if(JSON.typeof(dados) == "undefined"){
      Serial.println("Falha na entrada de dados");
      return;
    }
    Serial.print("JSON object = ");
    Serial.println(objeto);
    Serial.println("");
    res = false;
    }
}

String httpGETRequest(const char* host) {

  
  
  //conexao com a api
  WiFiClient client;
  HTTPClient http; 

  http.begin(client, host);
  
  // Requisiçao http GET 
  
  int httpResponseCode = http.GET();
  String resposta = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP codidgo de resposta: ");
    Serial.print(httpResponseCode);
    Serial.println("");
    resposta = http.getString();
  }
  else {
    Serial.print("Codigo de erro: ");
    Serial.println(httpResponseCode);
  }
 
  //  Cessando a conexao e Retornando valores 

  http.end();
  client.stop();
 
  return resposta;

  

  Serial.print("Falha na conexao com a api");
 

  
}

String httpPOSTRequest(const char* host){

  WiFiClient client;
  HTTPClient http;

  
  http.begin(client, host);
  
  //Enviando requisiçao POST

  http.addHeader("Content-Type","application/JSON");
  String body = "[{\"id\":1000,\"name\": \"Cesta  221\",\"items\":[{  \"id\":13,  \"name\": \"feijao\",  \"price\": 3.50,  \"weight\": 0.3}]}]";
  
  int httpResponseCode = http.POST(body);    

  Serial.println("");
  Serial.print("Codigo: ");
  Serial.print(httpResponseCode);
  Serial.println("");
  
  return "yes";
 

}

