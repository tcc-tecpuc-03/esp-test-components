#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

int codigoResposta;
String resposta;

class APIManager {
  private:
    const char* ssid = "DRIW_24Gh";
    const char* password = "17171407";

  public:
    APIManager(const char* ssid, const char* password) {
      this->ssid = ssid;
      this->password = password;
    }

    void begin() {
      Serial.begin(9600);
      WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Conectando ao WiFi...");
    }

    Serial.println("Conectado ao WiFi!");
    }

    String request(String url, String metodo, String dados) {
      HTTPClient http;
      WiFiClient client;      
      Serial.print("Realizando chamada para ");
      Serial.println(url);
      http.begin(client,url);
      if(metodo == "GET"){
        codigoResposta = http.GET();
        resposta = "";
      }else if(metodo == "POST"){
        http.addHeader("Content-Type","application/JSON");
        String dados = "{\"items\": [\"E8 A1 3A\"]}";
        codigoResposta = http.POST(dados);
        resposta = http.getString();        
      }
      
      Serial.println(codigoResposta);
      // Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(resposta).c_str());
      if (codigoResposta == HTTP_CODE_OK) {
        
        // Serial.print(dados);
        resposta = http.getString();
        Serial.print("Resposta recebida: ");
        Serial.println(resposta);
      } else {
        Serial.print("Falha na chamada. Código de resposta: ");
        Serial.println(codigoResposta);
      }
      http.end();
      return resposta;
    }
};