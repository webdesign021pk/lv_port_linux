#ifndef LAUNCHER_H
#define LAUNCHER_H

typedef void (*app_open_cb_t)(void); // Define the function pointer type

#include "launcher.h"
#include "../system/theme_manager/theme_manager.h"
#include "../system/app_registry/app_registry.h"

#include "lvgl.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> // For PATH_MAX

#include <cjson/cJSON.h>

void open_app_cb(lv_event_t * e)
{
    app_open_cb_t open_fn = (app_open_cb_t)lv_event_get_user_data(e);
    if(open_fn) open_fn(); // Call the dynamic function
}

void launcher_open(void)
{
    // Load apps from folders
    build_app_registry("src/apps");

    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    lv_obj_t * title = lv_label_create(scr);
    lv_label_set_text(title, "Launcher");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    uint8_t cols                    = 4;
    uint8_t margin                  = 10;
    uint8_t start_x                 = 10;
    uint8_t start_y                 = 40;

    uint8_t btn_w = current_theme.menu_app_grid_menu_icon_width;
    uint8_t btn_h = current_theme.menu_app_grid_menu_icon_height;
    lv_color_t icon_font_color = current_theme.menu_app_grid_menu_selected_font_color;


    for(int i = 0; i < app_count; i++) {
        u_int8_t row = i / cols;
        u_int8_t col = i % cols;

        // creating the button
        lv_obj_t * btn = lv_btn_create(scr);
        lv_obj_set_size(btn, btn_w, btn_h + 20);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + col * (btn_w + margin), start_y + row * (btn_h + 30));

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
                // printf("Image name: %s\n", app_registry[i].name);
                // printf("Zoom width: %d\n", zoom_w);
                // printf("Zoom height: %d\n", zoom_h);
                // printf("-------------------------\n");
                zoom = LV_MIN(zoom_w, zoom_h); // Pick smaller zoom to fit both W and H
                // printf("Zoom: %d\n", zoom);
                // Apply zoom to image
                lv_image_set_scale(img, zoom);
            }
        }

        lv_obj_align(img, LV_ALIGN_CENTER, 0, -10);

        // designing the label
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, app_registry[i].name);
        lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

        // Optional: limit width for wrapping
        lv_obj_set_width(label, btn_w); // wrap text if needed

        // Align to bottom center
        lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 5); // small offset from edge

        // Set font size and color
        // lv_obj_set_style_text_font(label, &my_font_12, 0); // Replace with your custom font
        lv_obj_set_style_text_color(label, icon_font_color, 0);

        lv_obj_add_event_cb(btn, open_app_cb, LV_EVENT_CLICKED, app_registry[i].exec_fn);
    }

    lv_scr_load(scr);
}

#endif // LAUNCHER_H
