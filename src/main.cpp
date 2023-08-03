#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "qrcode.h"
#include "ArduinoJson.h"
#include "WiFiClass.h"
#include "icons.h"

#define TFT_CS D8
#define TFT_RST D4
#define TFT_DC D3
#define TFT_SCLK D5
#define TFT_MOSI D7
#define SS_PIN D1
#define RST_PIN D0

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
MFRC522 rfid(SS_PIN, RST_PIN);
APIManager apiManager("ALHN-B945", "escola91148229");

int BasketId;
bool basketIdAssigned = false;                             // Variável para controlar se o BasketId já foi atribuído ou não
                                                           // id do carrinho (inicia nulo e pega apos criar o carrinho)
const String baseURL = "http://192.168.1.182:3003/api/v1"; // IPV4 DA SUA MAQUINA COM A PORTA DO BACKEND (3003)
const String ServerBaseURL = "http://192.168.1.182:3333";
unsigned long tagDetectedTime = 0;
bool tagDetected = false;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void createBasket()
{
  if (basketIdAssigned)
  {
    return; // Se o BasketId já foi atribuído, não faz nada e retorna
  }

  String postBasket = apiManager.request(
      (baseURL + "/basket").c_str(),
      "POST",
      "{\"rfid\": []}");

  DynamicJsonDocument basketJson(256);
  deserializeJson(basketJson, postBasket);
  BasketId = basketJson["id"];

  Serial.print(F("Basket ID: "));
  Serial.println(BasketId);

  basketIdAssigned = true;
}

void tftSetup()
{
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(4);
  tft.fillScreen(ST7735_WHITE);
}

void drawCard(const String &name, const String &price)
{
  int padding = 5;

  int cardX = padding;
  int cardY = padding;
  int cardWidth = tft.width() - (2 * padding);
  int cardHeight = tft.height() - (padding);

  uint16_t backgroundColor = ST7735_WHITE;
  uint16_t borderColor = ST7735_BLACK;
  uint16_t titleColor = ST7735_BLACK;
  uint16_t priceColor = ST7735_GREEN;

  tft.fillRoundRect(cardX, cardY, cardWidth, cardHeight, 10, backgroundColor);
  tft.drawRoundRect(cardX, cardY, cardWidth, cardHeight, 10, borderColor);

  int textX = cardX + padding;
  int textY = cardY + padding;

  int nameWidth = name.length() * 6;
  int nameX = textX + (cardWidth - nameWidth) / 2;

  tft.setTextColor(titleColor);
  tft.setTextSize(1);
  tft.setCursor(nameX, textY);
  tft.println(name);

  tft.setTextColor(priceColor);
  tft.setTextSize(2);
  tft.setCursor(textX, textY + 70);
  tft.println("R$ " + price);
}

void drawQRCode(const char *link)
{
  tft.fillScreen(ST7735_WHITE);

  const uint8_t ecc = 0;
  const uint8_t version = 3;

  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];

  qrcode_initText(&qrcode, qrcodeData, version, ecc, link);

  const int xy_scale = 3.8;
  const int x_offset = (tft.width() - xy_scale * qrcode.size) / 2;
  const int y_offset = (tft.height() - xy_scale * qrcode.size) / 2;

  for (uint8_t y = 0; y < qrcode.size; y++)
  {
    for (uint8_t x = 0; x < qrcode.size; x++)
    {
      bool mod = qrcode_getModule(&qrcode, x, y);

      if (mod)
      {
        int px = x_offset + (x * xy_scale);
        int py = y_offset + (y * xy_scale);
        tft.fillRect(px, py, xy_scale, xy_scale, ST7735_BLACK);
      }
    }
  }
}

void clearScreen()
{
  tft.fillScreen(ST7735_WHITE);

  int bitmapWidth = 64;
  int bitmapHeight = 64;

  int startX = (tft.width() - bitmapWidth) / 2;
  int startY = (tft.height() - bitmapHeight) / 2;

  tft.drawBitmap(startX, startY, bottle_icon, bitmapWidth, bitmapHeight, ST7735_GREEN);

  int textX = startX;
  int textY = startY + bitmapHeight;

  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(2);
  tft.setCursor(textX, textY);
  tft.println("Buysket");
}

void getTagId()
{
  if (!rfid.PICC_IsNewCardPresent())
  {
    if (tagDetected && (millis() - tagDetectedTime >= 10000))
    {
      tagDetected = false;
      clearScreen();
    }
    return;
  }

  if (!rfid.PICC_ReadCardSerial())
    return;

  String cardUID = "";
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    if (rfid.uid.uidByte[i] < 0x10)
      cardUID += "0";
    cardUID += String(rfid.uid.uidByte[i], HEX);
  }

  Serial.print(F("Card UID: "));
  Serial.println(cardUID);
  tagDetected = true;
  tagDetectedTime = millis();

  String itemData = apiManager.request((baseURL + "/items/" + cardUID).c_str(), "GET");
  const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(7) + 300;
  DynamicJsonDocument jsonDoc(bufferSize);

  String payload = "{\"basketId\": " + String(BasketId) + ", \"rfid\": [\"" + cardUID + "\"]}";
  apiManager.request((baseURL + "/basket").c_str(), "POST", payload.c_str());
  Serial.println("-----------[Payload]---------");
  Serial.println(payload);
  Serial.println("-----------------------------");

  Serial.println("-----------[Id carrinho]---------");
  Serial.println(BasketId);
  Serial.println("-----------------------------");

  DeserializationError error = deserializeJson(jsonDoc, itemData);

  if (error)
  {
    Serial.print(F("Falha ao analisar JSON: "));
    Serial.println(error.c_str());
    return;
  }

  JsonArray jsonArray = jsonDoc.as<JsonArray>();
  if (jsonArray.size() == 0)
  {
    Serial.println(F("Nenhum item encontrado."));
    return;
  }

  JsonObject item = jsonArray[0];
  String nome = item["nome"].as<String>();
  String preco = item["preco"].as<String>();

  Serial.print(F("Nome: "));
  Serial.println(nome);
  Serial.print(F("Preço: "));
  Serial.println(preco);

  drawCard(nome, preco);

  // rfid.PICC_HaltA();
  // rfid.PCD_StopCrypto1();
}

void setup(void)
{
  pinMode(D2, INPUT_PULLUP);
  Serial.begin(9600);
  SPI.begin();
  tftSetup();
  Serial.println(F("[!] - Sistema iniciado"));
  rfid.PCD_Init();
  Serial.println(F("[!] - RFID Iniciado..."));
  apiManager.begin();
  Serial.println(F("[!] - Wifi iniciado..."));

  Serial.println(F("[!] - Criando carrinho..."));
  createBasket();
  Serial.println("--------------------");
  clearScreen();
}

void loop()
{
  int reading = digitalRead(D2);

  if (reading == 0)
  {
    String qrLink = ServerBaseURL + "/cesta/" + String(BasketId);
    drawQRCode(qrLink.c_str());
  }

  getTagId();
}
