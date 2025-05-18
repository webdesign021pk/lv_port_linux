
// typedef void (*app_open_cb_t)(void); // Define the function pointer type

#include "launcher.h"
#include "../system/app_manager.h" // ✅ so we can use AppOpenFunc
#include "../system/theme_manager/theme_manager.h"
#include "../system/app_registry/app_registry.h"

#include "../system/screens/idle/home_screen/home_screen.h" // ✅ for home screen

#include "lvgl.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> // For PATH_MAX

#include <cjson/cJSON.h>

void open_app_cb(lv_event_t * e)
{
    AppOpenFunc open_fn = (AppOpenFunc)lv_event_get_user_data(e);
    if(open_fn) {
        app_open(open_fn); // ✅ delegate to app_manager
    }
}
static lv_obj_t * launcher_screen_internal = NULL;
static lv_obj_t * home_screen              = NULL;

static void menu_btn_cb(lv_event_t * e)
{
    lv_scr_load(home_screen_create());
}

void launcher_open(lv_obj_t ** out_launcher_screen)
{
    if(launcher_screen_internal) {
        *out_launcher_screen = launcher_screen_internal;
        return;
    }

    // Create only once
    lv_obj_t * scr           = lv_obj_create(NULL);
    launcher_screen_internal = scr;

    // Load apps from folders
    build_app_registry("src/apps");

    // defining the variables
    uint8_t cols    = 4;
    uint8_t margin  = 10;
    uint8_t start_x = 10;
    uint8_t start_y = 40;

    uint8_t btn_w                = current_theme.menu_app_grid_menu_icon_width;
    uint8_t btn_h                = current_theme.menu_app_grid_menu_icon_height;
    lv_color_t icon_font_color   = current_theme.menu_app_grid_menu_selected_font_color;
    lv_color_t menu_app_bg_color = current_theme.menu_app_wallpaper_color;
    uint8_t menu_app_bg_opa      = current_theme.menu_app_wallpaper_opacity;

    // Creating the screen
    // lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, menu_app_bg_color, 0);
    lv_obj_set_style_bg_opa(scr, menu_app_bg_opa, 0); // Fully transparent

    lv_obj_t * title = lv_label_create(scr);
    lv_label_set_text(title, "Launcher");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t * container = lv_obj_create(scr);
    lv_obj_set_size(container, LV_PCT(90), LV_PCT(75)); // Or hard size like 300x300
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 10);    // Slightly lower for aesthetics

    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP); // Enables row-wrapping
    lv_obj_set_flex_align(
        container, 
        LV_FLEX_ALIGN_CENTER,   // center icons horizontally in row
        LV_FLEX_ALIGN_CENTER,  // center icons horizontally in row
        LV_FLEX_ALIGN_START    // top icons vertically
    );
    lv_obj_set_style_pad_row(container, 15, 0);
    lv_obj_set_style_pad_column(container, 15, 0);
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0); // Transparent background

    for(int i = 0; i < app_count; i++) {
        u_int8_t row = i / cols;
        u_int8_t col = i % cols;

        // creating the button
        lv_obj_t * btn = lv_btn_create(container);
        lv_obj_set_size(btn, btn_w, btn_h + 20);
        // lv_obj_align(btn, LV_ALIGN_CENTER, start_x + col * (btn_w + margin), start_y + row * (btn_h + 30));
        // lv_obj_set_style_bg_color(btn, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0); // Fully transparent

        // designing the icon
        lv_obj_t * img = lv_image_create(btn);

        const char * icon_path     = app_registry[i].icon_path;
        const char * fallback_path = "A:/settings/theme/icon_default.png";

        lv_fs_file_t f;
        lv_fs_res_t file_res = lv_fs_open(&f, icon_path, LV_FS_MODE_RD);

        if(file_res == LV_FS_RES_OK) {
            lv_fs_close(&f); // Don't forget to close!
            lv_image_set_src(img, icon_path);
        } else {
            printf("Icon not found, using fallback: %s\n", fallback_path);
            lv_image_set_src(img, fallback_path);
        }

        // Get image source descriptor
        const lv_image_dsc_t * dsc = lv_image_get_src(img);

        // Get original image size
        int16_t src_w = lv_image_get_src_width(img);
        int16_t src_h = lv_image_get_src_height(img);

        // Only proceed if image has valid dimensions
        if(src_w > 0 && src_h > 0) {
            // Default zoom: 100% (LVGL uses 256 for 1x scale)
            uint16_t zoom = 256;

            // If image is larger than max size, calculate zoom
            if(src_w > btn_w || src_h > btn_h) {
                // Calculate scale factors using fixed-point math (256 = 100%)
                uint16_t zoom_w = (btn_w * 256 / src_w);
                uint16_t zoom_h = (btn_h * 256 / src_h);
                zoom            = LV_MIN(zoom_w, zoom_h); // Pick smaller zoom to fit both W and H
                // Apply zoom to image
                lv_image_set_scale(img, zoom);
            }
        }
        // Center the image horizontally and position it in the upper part of the button
        // printf("Image size: %d x %d\n", src_w, src_h);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -10); // move 10px towards top

        // designing the label
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, app_registry[i].name);

        // Center the label horizontally and position it below the image
        lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0); // 5px from bottom

        // Set text alignment to center (important for multi-line text)
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

        // Optional: limit width for wrapping
        lv_obj_set_width(label, btn_w - 10); // leave some padding

        // Set font size and color
        lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);
        lv_obj_set_style_text_color(label, icon_font_color, 0);

        lv_obj_add_event_cb(btn, open_app_cb, LV_EVENT_CLICKED, app_registry[i].exec_fn);
    }

    // lv_obj_t * test_box = lv_obj_create(scr);
    // lv_obj_set_size(test_box, 20, 20);
    // lv_obj_align(test_box, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_set_style_bg_color(test_box, lv_palette_main(LV_PALETTE_RED), 0);

    // Bottom Menu Button
    lv_obj_t * menu_btn = lv_btn_create(scr);
    lv_obj_align(menu_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_size(menu_btn, 70, 20);

    lv_obj_t * btn_label = lv_label_create(menu_btn);
    lv_label_set_text(btn_label, "Home");
    lv_obj_align(btn_label, LV_ALIGN_CENTER, 0, 0);

    // Set text alignment to center
    lv_obj_set_style_text_align(btn_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(btn_label, icon_font_color, 0);

    // On click, go back to home screen
    lv_obj_add_event_cb(menu_btn, menu_btn_cb, LV_EVENT_CLICKED, NULL);

    // Pass back the pointer
    *out_launcher_screen = scr;

    // lv_scr_load(scr);
}
