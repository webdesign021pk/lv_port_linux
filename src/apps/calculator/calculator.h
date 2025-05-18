#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "lvgl.h"

void calculator_open(lv_obj_t ** out_screen);
void calculator_close_cb(lv_event_t * e);
void calculator_minimize_cb(lv_event_t * e);

#endif
