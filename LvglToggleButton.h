#pragma once

#include "esphome.h"
#include "lvgl.h"

class LvglToggleButton : public Component, public Switch
{
private:
  lv_coord_t x;
  lv_coord_t y;
  lv_coord_t w;
  lv_coord_t h;

public:
  // constructor
  lv_obj_t *obj = NULL;
  LvglToggleButton(lv_coord_t _x, lv_coord_t _y, lv_coord_t _w, lv_coord_t _h)
  {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
  }

  void setup() override
  {
    // This will be called by App.setup()
    obj = lv_btn_create(lv_scr_act());
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE); // enable toggle

    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);

    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, (this)->get_name().c_str());
    lv_obj_center(label);

    // Set Callback
    lv_obj_add_event_cb(obj, lvgl_event_cb, LV_EVENT_VALUE_CHANGED, (void *)this);
  }

  void write_state(bool state) override
  {
    // This will be called every time the user requests a state change.
    ((state) ? lv_obj_add_state(obj, LV_STATE_CHECKED) : lv_obj_clear_state(obj, LV_STATE_CHECKED));

    // Acknowledge new state by publishing it
    publish_state(state);
  }

  static void lvgl_event_cb(lv_event_t *event)
  {
    lv_obj_t *target = lv_event_get_target(event);
    LvglToggleButton *sw = (LvglToggleButton *)event->user_data;

    // printf("Clicked\n");
    bool state = (lv_obj_get_state(target) & LV_STATE_CHECKED);

    // Acknowledge new state by publishing it
    sw->publish_state(state);
  }
};