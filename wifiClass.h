#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

class APIManager {
  private:
    const char* ssid;
    const char* password;

  public:
    APIManager(const char* ssid, const char* password) {
      this->ssid = ssid;
      this->password = password;
    }

    void begin() {
      Serial.begin(115200);
      WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
      }

      Serial.println("Conectado ao WiFi!");
    }

    String request(String url, String metodo) {
      HTTPClient http;
      Serial.print("Realizando chamada para ");
      Serial.println(url);
      http.begin(url);
      int codigoResposta = http.sendRequest(metodo);
      String resposta = "";
      if (codigoResposta == HTTP_CODE_OK) {
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
