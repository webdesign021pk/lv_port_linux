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
#include "system/screens/idle/home_screen/home_screen.h"

// for root directory start
#define _POSIX_C_SOURCE 200809L  // For modern POSIX features
#include <fcntl.h>               // For O_* flags
// #include <unistd.h>              // For POSIX file operations
#include <dirent.h>              // For directory operations
#include <sys/stat.h>            // For file stats
#include <errno.h>               // For error handling
// #include <stdio.h>               // For printf
// #include <string.h>              // For strcpy
// for root directory end
#define ICON_CACHE_FMT "%s/.cache/launcher/icons/hicolor/48x48/apps"

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

static bool fs_linux_ready(lv_fs_drv_t * drv)
{
    (void)drv;   // Unused
    return true; // Always ready on Linux
}

static void * fs_linux_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    (void)drv; // Unused

    int flags = 0;
    if(mode == LV_FS_MODE_WR)
        flags = O_WRONLY | O_CREAT;
    else if(mode == LV_FS_MODE_RD)
        flags = O_RDONLY;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
        flags = O_RDWR | O_CREAT;

    int fd = open(path, flags, 0666);
    return (fd >= 0) ? (void *)(intptr_t)fd : NULL;
}

static lv_fs_res_t fs_linux_close(lv_fs_drv_t * drv, void * file_p)
{
    (void)drv; // Unused
    close((int)(intptr_t)file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_linux_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    (void)drv; // Unused
    *br = read((int)(intptr_t)file_p, buf, btr);
    return (*br <= 0) ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t fs_linux_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    (void)drv; // Unused
    off_t offset = lseek((int)(intptr_t)file_p, pos, whence);
    return (offset == (off_t)-1) ? LV_FS_RES_FS_ERR : LV_FS_RES_OK;
}

static lv_fs_res_t fs_linux_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    (void)drv; // Unused
    *pos_p = lseek((int)(intptr_t)file_p, 0, SEEK_CUR);
    return (*pos_p == (uint32_t)-1) ? LV_FS_RES_FS_ERR : LV_FS_RES_OK;
}

void ensure_icon_cache_exists()
{
    char path[512];
    snprintf(path, sizeof(path), ICON_CACHE_FMT, getenv("HOME"));
    char cmd[768];
    snprintf(cmd, sizeof(cmd), "mkdir -p '%s'", path);
    system(cmd);
}

int main(int argc, char ** argv)
{
    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize file system drivers */
    lv_fs_init();       // essential to register "A:/"
    lv_fs_if_init();    // 🚨 This is missing in your `main.c`
    lv_fs_posix_init(); // essential to register "A:/ to read file like app png icons"

    /* Set up the filesystem driver */
    lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /* Set up the 'S:' drive */
    fs_drv.letter   = 'S';
    fs_drv.ready_cb = fs_linux_ready;
    fs_drv.open_cb  = fs_linux_open;
    fs_drv.close_cb = fs_linux_close;
    fs_drv.read_cb  = fs_linux_read;
    fs_drv.seek_cb  = fs_linux_seek;
    fs_drv.tell_cb  = fs_linux_tell;

    lv_fs_drv_register(&fs_drv);

    // initialize the launcher icon directory
    ensure_icon_cache_exists();

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

    // loading the theme
    theme_manager_load();

    lv_scr_load(home_screen_create());

    // app_manager_start();  // Start the system (loads launcher)

    // driver loop
    driver_backends_run_loop();

    return 0;
}
