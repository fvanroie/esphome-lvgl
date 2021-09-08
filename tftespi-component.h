#pragma once
#include "esphome.h"
#include "tft_espi.h"
#include "bootlogo.h"
#include "lvgl.h" // for lv_color_make

TFT_eSPI tft;

class TFTeSPIComponent : public Component
{
public:
  void setup() override
  {
    // This will be called once to set up the component
    // think of it as the setup() call in Arduino
    tft.begin();
    tft.setSwapBytes(true); /* set endianess */
    tft.setRotation(TFT_ROTATION);
    splashscreen();
    uint16_t calData[5] = {TOUCH_CAL_DATA};
    tft.setTouch(calData);

    delay(250);
  }
  void IRAM_ATTR loop() override
  {
    // This will be called very often after setup time.
    // think of it as the loop() call in Arduino
  }
  float get_setup_priority() const override { return esphome::setup_priority::HARDWARE; }

private:
  void splashscreen()
  {
    uint8_t fg[] = logoFgColor;
    uint8_t bg[] = logoBgColor;
    lv_color_t fgColor = lv_color_make(fg[0], fg[1], fg[2]);
    lv_color_t bgColor = lv_color_make(bg[0], bg[1], bg[2]);

    tft.fillScreen(bgColor.full);
    int x = (tft.width() - logoWidth) / 2;
    int y = (tft.height() - logoHeight) / 2;
    tft.drawXBitmap(x, y, logoImage, logoWidth, logoHeight, fgColor.full);
  }
};