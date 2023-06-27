#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "qrcode.h"
// Class
#include "WiFiClass.h"

// rc522 pins
// RST (Reset) <-> D0
// MISO (Master Input Slave Output) <-> D6
// MOSI (Master Output Slave Input) <-> D7
// SCK (Serial Clock) <-> D5
// SS/SDA (Slave select) <-> D1

// display pins (Adafruit_ST7735)
// VCC <-> 3.3V
// GND <-> GND
// CS <-> D8
// RST <-> D4
// DC/A0 <-> D3
// SDA/MOSI <-> D7
// SCK <-> D5
// LED <-> 3.3V

#define TFT_CS D8
#define TFT_RST D4
#define TFT_DC D3
#define TFT_SCLK D5
#define TFT_MOSI D7
#define SS_PIN D1
#define RST_PIN D0

MFRC522 rfid(SS_PIN, RST_PIN);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
APIManager apiManager("ALHN-B945", "escola91148229");

unsigned long tagDetectedTime = 0;
bool tagDetected = false;

void tftSetup()
{
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(4);
  tft.fillScreen(ST7735_WHITE);
}
void clearScreen()
{
  tft.fillScreen(ST7735_BLACK);
}
void drawCard(const String &name, const String &value)
{
  // Defina o espaçamento
  int padding = 5;

  // Defina as coordenadas e as dimensões do card
  int cardX = padding;
  int cardY = padding;
  int cardWidth = tft.width() - (2 * padding);
  int cardHeight = tft.height() - (padding);

  // Cores personalizadas
  uint16_t backgroundColor = ST7735_WHITE;
  uint16_t borderColor = ST7735_BLACK;
  uint16_t titleColor = ST7735_BLACK;
  uint16_t valueColor = ST7735_BLUE;
  uint16_t priceColor = ST7735_GREEN;

  // Desenhe o retângulo do card com bordas arredondadas
  tft.fillRoundRect(cardX, cardY, cardWidth, cardHeight, 10, backgroundColor);
  tft.drawRoundRect(cardX, cardY, cardWidth, cardHeight, 10, borderColor);

  // Defina as coordenadas do texto
  int textX = cardX + padding;
  int textY = cardY + padding;

  // Calcule a posição horizontal para centralizar o texto
  int nameWidth = name.length() * 6; // Largura aproximada do texto com tamanho de fonte 1
  int nameX = textX + (cardWidth - nameWidth) / 2;

  // Escreva o nome no card
  tft.setTextColor(titleColor);
  tft.setTextSize(1);
  tft.setCursor(nameX, textY);
  tft.println(name);

  // Escreva o valor no card
  tft.setTextColor(valueColor);
  tft.setTextSize(1);
  tft.setCursor(textX, textY + 35);
  tft.println(value);

  // Escreva o preço no card
  tft.setTextColor(priceColor);
  tft.setTextSize(2);
  tft.setCursor(textX, textY + 70);
  tft.println("R$ 0,00");
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
  drawCard("ITEM I", cardUID);

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void drawQRCode(const char *link)
{
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

void setup(void)
{
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  // apiManager.begin();
  tftSetup();
  Serial.println(F("[!] - Sistema iniciado"));
}

void loop()
{
  getTagId();
}