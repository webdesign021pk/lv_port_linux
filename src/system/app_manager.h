#ifndef APP_MANAGER_H
#define APP_MANAGER_H

#include "lvgl.h"  // Required because we're passing lv_obj_t* in function signature

// Add this typedef so the compiler knows what AppOpenFunc is
typedef void (*AppOpenFunc)(lv_obj_t ** out_screen);

void app_manager_start(void);
void app_open(AppOpenFunc fn);
void app_minimize(void);
void app_close(void);

#endif
