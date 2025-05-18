#ifndef SMS_H
#define SMS_H

#include "lvgl.h"

void sms_open(lv_obj_t ** out_screen);
void sms_close_cb(lv_event_t * e);

#endif
