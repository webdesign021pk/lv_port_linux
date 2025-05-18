#include "notes.h"
#include "system/app_manager.h"  // ✅ You now call app_close() from here

void notes_open(lv_obj_t ** out_screen)
{
    lv_obj_t * scr = lv_obj_create(NULL);  // Create app screen
    lv_obj_set_style_bg_color(scr, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);

    // Title label
    lv_obj_t * title = lv_label_create(scr);
    lv_label_set_text(title, "NOTES App");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Close button
    lv_obj_t * close_btn = lv_btn_create(scr);
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "X");
    lv_obj_add_event_cb(close_btn, notes_close_cb, LV_EVENT_CLICKED, NULL);

    *out_screen = scr;  // ✅ Return screen to app_manager
}

void notes_close_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    app_close();  // ✅ Delegate screen cleanup to app_manager
}
