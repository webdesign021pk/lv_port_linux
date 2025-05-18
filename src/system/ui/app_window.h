#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include "lvgl.h"

typedef struct {
    lv_obj_t * screen;
    lv_obj_t * content;
} AppWindow;

AppWindow app_window_create(const char * title);

void app_close_cb(lv_event_t * e);
void app_minimize_cb(lv_event_t * e);

#endif // APP_WINDOW_H
