/*******************************************************************
 *
 * main.c - LVGL simulator for GNU/Linux
 *
 * Based on the original file from the repository
 *
 * @note eventually this file won't contain a main function and will
 * become a library supporting all major operating systems
 *
 * To see how each driver is initialized check the
 * 'src/lib/display_backends' directory
 *
 * - Clean up
 * - Support for multiple backends at once
 *   2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lvgl/lvgl.h"

#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

// #include "sdl/sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mousewheel.h"
#include "lvgl/src/drivers/sdl/lv_sdl_keyboard.h"

// #include "launcher/launcher.h"
#include "system/app_manager.h"

/* Internal functions */
static void configure_simulator(int argc, char ** argv);

/* contains the name of the selected backend if user
 * has specified one on the command line */
static char * selected_backend;

/* Global simulator settings, defined in lv_linux_backend.c */
extern simulator_settings_t settings;

/**
 * @brief Configure simulator
 * @description process arguments recieved by the program to select
 * appropriate options
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
static void configure_simulator(int argc, char ** argv)
{
    int opt = 0;
    char * backend_name;

    selected_backend = NULL;
    driver_backends_register();

    /* Default values */
    settings.window_width  = atoi(getenv("LV_SIM_WINDOW_WIDTH") ?: "800");
    settings.window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ?: "480");
}

/**
 * custom demo function
 */

void close_popup_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * popup = lv_obj_get_parent(btn);
    lv_obj_del(popup);  // This deletes the whole popup (including the button)
}

void my_button_event_handler(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_user_data(e);
    const char * txt = lv_textarea_get_text(ta);

    // 🔧 Create toast/popup container
    lv_obj_t * popup = lv_obj_create(lv_scr_act());
    lv_obj_set_size(popup, 220, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(popup, 10, 0);
    lv_obj_set_style_radius(popup, 10, 0);
    lv_obj_center(popup);  // or use lv_obj_align() for bottom
    lv_obj_set_style_bg_color(popup, lv_palette_main(LV_PALETTE_BLUE), 0);

    // 📝 Create label with message
    lv_obj_t * msg = lv_label_create(popup);
    lv_label_set_text_fmt(msg, "Submitted: %s", txt);
    lv_obj_set_width(msg, 180);

    // ❌ Add a close button
    lv_obj_t * close_btn = lv_btn_create(popup);
    lv_obj_set_size(close_btn, 60, 30);
    lv_obj_align(close_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "Close");

    // 🔁 Close popup on button click
    lv_obj_add_event_cb(close_btn, close_popup_event_cb, LV_EVENT_CLICKED, NULL);

}

void create_custom_demo(void)
{
    lv_obj_t * scr = lv_scr_act();

    // ✅ Create text area
    lv_obj_t * ta = lv_textarea_create(scr);
    lv_textarea_set_placeholder_text(ta, "Enter your name");
    lv_obj_set_width(ta, 200);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 20);

    // ✅ Create button
    lv_obj_t * btn = lv_btn_create(scr);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 40);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Submit");

    // ✅ Handle button click
    lv_obj_add_event_cb(btn, my_button_event_handler, LV_EVENT_CLICKED, ta);

    // ✅ Setup group and keyboard focus
    lv_group_t * g = lv_group_create();
    lv_group_add_obj(g, ta);
    lv_indev_t * kb = lv_indev_get_next(NULL);  // Get first input device
    lv_indev_set_group(kb, g);
    lv_group_focus_obj(ta);
}


/**
 * @brief entry point
 * @description start a demo
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
int main(int argc, char ** argv)
{
    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend */
    if(driver_backends_init_backend(selected_backend) == -1) {
        die("Failed to initialize display backend");
    }

    #if LV_USE_EVDEV
        if(driver_backends_init_backend("EVDEV") == -1) {
            die("Failed to initialize evdev");
        }
    #endif

    // ✅ Input devices
    lv_sdl_mouse_create();
    lv_indev_t * kb = lv_sdl_keyboard_create();

    // making the custom demo function appear
    // create_custom_demo();
    // launcher_open();
    app_manager_start();  // 👈 Start the system (loads launcher)


    // driver loop
    driver_backends_run_loop();

    return 0;
}
