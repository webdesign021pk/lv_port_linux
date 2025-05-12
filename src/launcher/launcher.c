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

    int cols    = 4;
    int margin  = 10;
    int start_x = 10;
    int start_y = 40;
    int btn_w   = 70;
    int btn_h   = 70;

    for(int i = 0; i < app_count; i++) {
        int row = i / cols;
        int col = i % cols;

        // creating the button
        lv_obj_t * btn = lv_btn_create(scr);
        lv_obj_set_size(btn, btn_w, btn_h + 20);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + col * (btn_w + margin), start_y + row * (btn_h + 30));

        // designing the icon
        lv_obj_t * img = lv_image_create(btn);
        lv_image_set_src(img, app_registry[i].icon_path);
        lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);

        const char * icon_path     = app_registry[i].icon_path;
        const char * fallback_path = "A:/settings/theme/icon_default.png";

        lv_fs_file_t f;
        lv_fs_res_t file_res = lv_fs_open(&f, icon_path, LV_FS_MODE_RD);
        lv_obj_set_size(img, 48, 48);  // or whatever consistent size you prefer
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -10);  // center horizontally, slightly up

        if(file_res == LV_FS_RES_OK) {
            lv_fs_close(&f); // Don't forget to close!
            lv_img_set_src(img, icon_path);
        } else {
            printf("Icon not found, using fallback: %s\n", fallback_path);
            lv_img_set_src(img, fallback_path);
        }

        // designing the label
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, app_registry[i].name);
        lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

        lv_obj_add_event_cb(btn, open_app_cb, LV_EVENT_CLICKED, app_registry[i].exec_fn);
    }

    lv_scr_load(scr);
}

#endif // LAUNCHER_H
