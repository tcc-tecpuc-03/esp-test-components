#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <qrcode.h>

#define TFT_CS   D1
#define TFT_RST  D3
#define TFT_DC   D4
#define TFT_SCK  D5
#define TFT_MOSI D7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

void tftSetup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(2);
  tft.fillScreen(ST7735_WHITE);
}

void drawQRCode() {
  const char *data = "http://google.com/";
  const uint8_t ecc = 0; 
  const uint8_t version = 3;

  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];

  qrcode_initText(&qrcode, qrcodeData, version, ecc, data);

  const int xy_scale = 3.8;
  const int x_offset = (tft.width() - xy_scale*qrcode.size)/2;
  const int y_offset = (tft.height() - xy_scale*qrcode.size)/2;

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      bool mod = qrcode_getModule(&qrcode, x, y);

      if (mod) {
        int px = x_offset + (x * xy_scale);
        int py = y_offset + (y * xy_scale);
        tft.fillRect(px, py, xy_scale, xy_scale, ST7735_BLACK);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("- setup() started -");

  tftSetup();
}

void loop() {
  drawQRCode();
}
