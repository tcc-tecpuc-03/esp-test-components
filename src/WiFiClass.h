#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

class APIManager
{
private:
  const char *ssid;
  const char *password;

public:
  APIManager(const char *ssid, const char *password)
  {
    this->ssid = ssid;
    this->password = password;
  }

  void begin()
  {
    Serial.begin(9600);
    WiFi.begin(ssid, password);

    Serial.println("Conectando ao WiFi.");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("-------------------------");
    Serial.println("Conectado ao WiFi!");
    Serial.println("Endereço de IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("Endereço MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.println("Tempo de execução: ");
    Serial.println(millis());
    Serial.println("-------------------------");
  }

  String request(const char *url, const char *metodo, const char *jsonBody = "")
  {
    HTTPClient http;
    WiFiClient client;

    Serial.println("-----------[" + String(metodo) + "]-----------");
    Serial.print("URL: ");
    Serial.println(url);
    Serial.print("Body: ");
    Serial.println(jsonBody);
    Serial.println("----------------------------------");

    http.begin(client, url);

    http.addHeader("Content-Type", "application/json");

    int codigoResposta = http.sendRequest(metodo, jsonBody);
    String resposta = "";

    if (codigoResposta == HTTP_CODE_OK)
    {
      resposta = http.getString();
      Serial.println("-----------[" + String(metodo) + "]-----------");
      Serial.print("Resposta recebida: ");
      Serial.println(resposta);
    }
    else
    {
      Serial.print("Falha na chamada. Código de resposta: ");
      Serial.println(codigoResposta);
    }

    http.end();
    return resposta;
  }
};
