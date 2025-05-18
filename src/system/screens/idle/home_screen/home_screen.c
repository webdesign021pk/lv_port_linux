// home_screen.c
#include "home_screen.h"
#include "../../../theme_manager/theme_manager.h"
#include "../../../app_manager.h" // for launching apps if needed
#include "../../app_menu/launcher/launcher.h"

static lv_obj_t * home_screen = NULL;

static void menu_btn_cb(lv_event_t * e)
{
    // lv_obj_t * launcher;
    // launcher_open(&launcher);
    // lv_scr_load(launcher);
    app_manager_start();  // Start the system (loads launcher)

}

lv_obj_t * home_screen_create(void)
{
    if(home_screen) return home_screen;

    home_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(home_screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(home_screen, LV_OPA_COVER, 0);

    // Welcome Label (or clock)
    lv_obj_t * label = lv_label_create(home_screen);
    lv_label_set_text(label, "Welcome to SymbianOS");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -40);

    // Bottom Menu Button
    lv_obj_t * menu_btn = lv_btn_create(home_screen);
    lv_obj_align(menu_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_size(menu_btn, 100, 40);

    lv_obj_t * btn_label = lv_label_create(menu_btn);
    lv_label_set_text(btn_label, "Menu");

    lv_obj_add_event_cb(menu_btn, menu_btn_cb, LV_EVENT_CLICKED, NULL);

    return home_screen;
}
