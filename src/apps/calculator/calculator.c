#include "calculator.h"
#include "system/app_manager.h"  // ✅ You now call app_close() from here
#include "system/ui/app_window.h"

/**
 * 🔁 Close popup button callback
 */
void close_popup_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * popup = lv_obj_get_parent(btn);
    lv_obj_del(popup);  // Delete the popup container
}

/**
 * 📥 Handle Submit button event
 */
void my_button_event_handler(lv_event_t * e)
{
    lv_obj_t * ta = (lv_obj_t *)lv_event_get_user_data(e);
    const char * txt = lv_textarea_get_text(ta);

    // 🔧 Create popup container
    // lv_obj_t * popup = lv_obj_create(lv_layer_top());

    lv_obj_t * popup = lv_obj_create(lv_scr_act());
    lv_obj_set_size(popup, 220, LV_SIZE_CONTENT);
    
    lv_obj_set_style_pad_all(popup, 10, 0);
    lv_obj_set_style_radius(popup, 10, 0);
    lv_obj_set_style_bg_color(popup, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_center(popup);

    // 📝 Add message
    lv_obj_t * msg = lv_label_create(popup);
    lv_label_set_text_fmt(msg, "Submitted: %s", txt);
    lv_obj_set_width(msg, 180);

    // ❌ Close button
    lv_obj_t * close_btn = lv_btn_create(popup);
    lv_obj_set_size(close_btn, 60, 30);
    lv_obj_align(close_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "Close");

    // 📌 Attach close event
    lv_obj_add_event_cb(close_btn, close_popup_event_cb, LV_EVENT_CLICKED, NULL);
}

/**
 * 🧪 Setup UI for input and popup demo
 */
void create_custom_demo(lv_obj_t * scr)
{
    // ✅ Create text area
    lv_obj_t * ta = lv_textarea_create(scr);
    lv_textarea_set_placeholder_text(ta, "Enter your name");
    lv_obj_set_width(ta, 200);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 50);

    // ✅ Create submit button
    lv_obj_t * btn = lv_btn_create(scr);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 40);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Submit");

    // ✅ Bind event
    lv_obj_add_event_cb(btn, my_button_event_handler, LV_EVENT_CLICKED, ta);

    // Optional: Input group for keyboard devices (if you have focus/navigation logic)
    lv_group_t * g = lv_group_create();
    lv_group_add_obj(g, ta);

    lv_indev_t * kb = lv_indev_get_next(NULL);
    if (kb) lv_indev_set_group(kb, g);

    lv_group_focus_obj(ta);
}

void calculator_open(lv_obj_t ** out_screen)
{
    AppWindow win = app_window_create("Calculator");

    // Add your UI to `win.content` instead of `scr`
    create_custom_demo(win.content);

    *out_screen = win.screen;
}
