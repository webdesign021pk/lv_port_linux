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

#include "lib/lv_fs_if/lv_fs_if.h" 


#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

// #include "sdl/sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mousewheel.h"
#include "lvgl/src/drivers/sdl/lv_sdl_keyboard.h"

// #include "launcher/launcher.h"
#include "system/app_manager.h"
#include "system/theme_manager/theme_manager.h"

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
    
    /* Initialize file system drivers */
    lv_fs_init();  // essential to register "A:/"
    lv_fs_if_init();  // 🚨 This is missing in your `main.c`
    lv_fs_posix_init(); // essential to register "A:/ to read file like app png icons"

    /* Initialize the configured backend */
    if(driver_backends_init_backend(selected_backend) == -1) {
        die("Failed to initialize display backend");
    }

    #if LV_USE_EVDEV
        if(driver_backends_init_backend("EVDEV") == -1) {
            die("Failed to initialize evdev");
        }
    #endif

    // Input devices
    lv_sdl_mouse_create();
    lv_indev_t * kb = lv_sdl_keyboard_create();

    //loading the theme
    theme_manager_load();

    app_manager_start();  // Start the system (loads launcher)

    // driver loop
    driver_backends_run_loop();

    return 0;
}
