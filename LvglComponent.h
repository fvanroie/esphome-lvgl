#pragma once

#include "esphome.h"
#include "lvgl.h"
#include "lv_demo.h"
#include "TFT_eSPI.h"
#include "bootlogo.h"

const size_t buf_pix_count = LV_HOR_RES_MAX * LV_VER_RES_MAX / 5;

static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[buf_pix_count];
lv_style_t switch_style;

/* LVGL callbacks - Needs to be accessible from C library */
void IRAM_ATTR my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void IRAM_ATTR gui_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

TFT_eSPI tft;

class LvglComponent : public Component
{
public:
  void setup() override
  {
    // This will be called once to set up the component
    // think of it as the setup() call in Arduino
    tft_setup();
    lv_init();

#if USE_LV_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

    lv_disp_draw_buf_init(&disp_buf, buf, NULL, buf_pix_count);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = gui_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // Make unchecked checkboxes darker grey
    lv_style_init(&switch_style);
    lv_style_set_bg_color(&switch_style, lv_palette_main(LV_PALETTE_GREY));

    // lv_demo_widgets();
    // lv_demo_music();

    this->high_freq_.start(); // avoid 16 ms delay
  }
  void IRAM_ATTR loop() override
  {
    // This will be called every "update_interval" milliseconds.
    lv_timer_handler(); // called by dispatch_loop
    // this->high_freq_.stop();  // decrease the counter for check
    // if (high_freq_num_requests == 1)
    //   delay(5);
    // this->high_freq_.start();  // increase the counter for main loop
  }
  float get_setup_priority() const override { return esphome::setup_priority::DATA; }

private:
  /// High Frequency loop() requester used during sampling phase.
  HighFrequencyLoopRequester high_freq_;

  void tft_setup()
  {
    // This will be called once to set up the component
    // think of it as the setup() call in Arduino
    tft.begin();
    tft.setSwapBytes(true); /* set endianess */
    tft.setRotation(TFT_ROTATION);
    tft_splashscreen();
    uint16_t calData[5] = {TOUCH_CAL_DATA};
    tft.setTouch(calData);

    delay(250);
  }

  void tft_splashscreen()
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

/* Update the TFT - Needs to be accessible from C library */
void IRAM_ATTR gui_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  size_t len = lv_area_get_size(area);

  /* Update TFT */
  tft.startWrite();                                      /* Start new TFT transaction */
  tft.setWindow(area->x1, area->y1, area->x2, area->y2); /* set the working window */
#ifdef USE_DMA_TO_TFT
  tft.pushPixelsDMA((uint16_t *)color_p, len); /* Write words at once */
#else
  tft.pushPixels((uint16_t *)color_p, len); /* Write words at once */
#endif
  tft.endWrite(); /* terminate TFT transaction */

  /* Tell lvgl that flushing is done */
  lv_disp_flush_ready(disp);
}

/*Read the touchpad - Needs to be accessible from C library */
void IRAM_ATTR my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;

  bool touched = tft.getTouch(&touchX, &touchY, 600);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    // Serial.print("Data x");
    // Serial.print(touchX);

    // Serial.print(" - y");
    // Serial.println(touchY);
  }
}
