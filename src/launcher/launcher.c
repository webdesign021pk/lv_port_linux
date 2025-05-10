#ifndef LAUNCHER_H
#define LAUNCHER_H

typedef void (*app_open_cb_t)(void);  // Define the function pointer type

#include "launcher.h"
#include "../apps/sms/sms.h"
#include "../apps/notes/notes.h"
#include "../apps/calculator/calculator.h"
#include "../system/theme_manager/theme_manager.h"

void open_app_cb(lv_event_t * e)
{
    app_open_cb_t open_fn = (app_open_cb_t)lv_event_get_user_data(e);
    if(open_fn) open_fn();  // Call the app's open function
}


void launcher_open(void)
{
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    lv_obj_t * title = lv_label_create(scr);
    lv_label_set_text(title, "Launcher");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Button: SMS
    lv_obj_t * btn_sms = lv_btn_create(scr);
    lv_obj_align(btn_sms, LV_ALIGN_CENTER, -80, 40);
    lv_obj_t * lbl_sms = lv_label_create(btn_sms);
    lv_label_set_text(lbl_sms, "SMS");
    lv_obj_add_event_cb(btn_sms, open_app_cb, LV_EVENT_CLICKED, sms_open);  // 👈 pass function pointer

    // Button: Notes
    lv_obj_t * btn_notes = lv_btn_create(scr);
    lv_obj_align(btn_notes, LV_ALIGN_CENTER, 0, 40);
    lv_obj_t * lbl_notes = lv_label_create(btn_notes);
    lv_label_set_text(lbl_notes, "Notes");
    lv_obj_add_event_cb(btn_notes, open_app_cb, LV_EVENT_CLICKED, notes_open);  // 👈 pass function pointer

    // Button: Calculator
    lv_obj_t * btn_calc = lv_btn_create(scr);
    lv_obj_align(btn_calc, LV_ALIGN_CENTER, 80, 40);
    lv_obj_t * lbl_calc = lv_label_create(btn_calc);
    lv_label_set_text(lbl_calc, "Calc");
    lv_obj_add_event_cb(btn_calc, open_app_cb, LV_EVENT_CLICKED, calculator_open);  // 👈 pass function pointer
    
    // Button: reload theme
    lv_obj_t * btn_theme = lv_btn_create(scr);
    lv_obj_align(btn_theme, LV_ALIGN_CENTER, 0, 100);
    lv_obj_t * lbl_theme = lv_label_create(btn_theme);
    lv_label_set_text(lbl_theme, "Reload Theme");
    lv_obj_add_event_cb(btn_theme, open_app_cb, LV_EVENT_CLICKED, theme_manager_load);  // 👈 pass function pointer

    lv_scr_load(scr);
}

#endif // LAUNCHER_H
