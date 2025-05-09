#include "calculator.h"
#include "../launcher/launcher.h"  // or declare externally if needed

static lv_obj_t * app_screen = NULL;
extern void launcher_open(void);  // 👈 declare this at the top if needed

void calculator_open(void)
{
    app_screen = lv_obj_create(NULL); // Create new screen
    lv_obj_set_style_bg_color(app_screen, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);

    // Title label
    lv_obj_t * title = lv_label_create(app_screen);
    lv_label_set_text(title, "CALCULATOR App");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Close button
    lv_obj_t * close_btn = lv_btn_create(app_screen);
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "X");

    lv_obj_add_event_cb(close_btn, calculator_close_cb, LV_EVENT_CLICKED, NULL);

    // Load this screen
    lv_scr_load(app_screen);
}

void calculator_close_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    launcher_open();  // Go back to home screen
}
