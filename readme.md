## BUYSKET

### Introdução
Este repositório contém o código para utilizar o display QRCODE com o microcontrolador ESP. O display utiliza as seguintes portas:

| Display QRCODE | ESP        |
| -------------- | ----------|
| VCC            | 3.3v       |
| GND            | GND        |
| CS             | D1         |
| RESET          | RST        |
| D/C            | D4         |
| SDI(MOSI)      | D7         |
| SCK            | D5         |
| LED            | 3.3V       |

# Exemplo de uso da WifiClass

Este código é um exemplo de uso de uma `APIManager` para fazer uma solicitação para uma API externa. Ele utiliza uma conexão Wi-Fi para se conectar à rede e envia uma solicitação HTTP utilizando o método PUT para um endpoint específico. Em seguida, aguarda 5 segundos antes de realizar outra chamada. 

Para utilizar este exemplo, é importante substituir `SUA_REDE_WIFI` e `SUA_SENHA_WIFI` com as credenciais corretas da sua rede Wi-Fi. Também é importante alterar a URL e o payload dos dados de acordo com a API que está sendo usada.

```cpp
APIManager apiManager("SUA_REDE_WIFI", "SUA_SENHA_WIFI");

void setup() {
  apiManager.begin();
}

void loop() {
  String url = "http://exemplo.com/api";
  String metodo = "PUT";
  String dados = "{\"nome\":\"João\",\"idade\":30}"; // Dados em formato JSON

  String resposta = apiManager.request(url, metodo, dados);
  Serial.println(resposta);
  delay(5000); // Aguarda 5 segundos antes de realizar outra chamada
}
```
