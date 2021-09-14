#pragma once

#include "esphome.h"
#include "lvgl.h"

class LvglCheckbox : public Component, public Switch
{
private:
  lv_coord_t x;
  lv_coord_t y;
  lv_coord_t w;
  lv_coord_t h;

public:
  // constructor
  lv_obj_t *obj = NULL;
  LvglCheckbox(lv_coord_t _x, lv_coord_t _y, lv_coord_t _w, lv_coord_t _h)
  {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
  }

  void setup() override
  {
    // This will be called by App.setup()
    obj = lv_checkbox_create(lv_scr_act());
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_checkbox_set_text(obj, (this)->get_name().c_str());

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
    LvglCheckbox *sw = (LvglCheckbox *)event->user_data;

    // printf("Clicked\n");
    bool state = (lv_obj_get_state(target) & LV_STATE_CHECKED);

    // Acknowledge new state by publishing it
    sw->publish_state(state);
  }
};