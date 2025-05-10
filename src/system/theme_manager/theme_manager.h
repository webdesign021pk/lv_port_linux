// theme_manager.h
#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void theme_manager_load(void);
void theme_manager_apply(const char * theme_name);
void theme_manager_save(void);

extern struct theme_data_t {
    char name[32];
    lv_color_t bg_color;
    lv_color_t btn_color;
    lv_color_t text_color;
    char icon_sms[64];
    char icon_notes[64];
    char icon_calculator[64];
    char wallpaper[64];
} current_theme;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // THEME_MANAGER_H
