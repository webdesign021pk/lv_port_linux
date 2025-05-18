// app_manager.c
#include "app_manager.h"
#include "screens/idle/home_screen/home_screen.h"
#include "screens/app_menu/launcher/launcher.h"

// #include "screens/app_menu/launcher/launcher.h"
#include "debug/debug.c"

// Forward declaration for dynamic apps
typedef void (*AppOpenFunc)(lv_obj_t ** out_screen);

static lv_obj_t * launcher_screen = NULL;
static lv_obj_t * app_screen      = NULL;
static AppOpenFunc current_app_fn = NULL;

typedef struct
{
    AppOpenFunc fn;
    lv_obj_t * screen;
} AppRecord;

#define MAX_APPS 10
static AppRecord app_registry[MAX_APPS];
static int app_registry_count = 0;

static AppRecord * current_app = NULL;

AppRecord * find_or_create_app(AppOpenFunc fn)
{
    for(int i = 0; i < app_registry_count; i++) {
        if(app_registry[i].fn == fn) return &app_registry[i];
    }

    if(app_registry_count >= MAX_APPS) return NULL; // Max limit

    AppRecord * new_app = &app_registry[app_registry_count++];
    new_app->fn         = fn;
    new_app->screen     = NULL;
    return new_app;
}

void app_manager_start(void)
{

    // Start with a blank screen to flush the framebuffer
    // This is important to avoid flickering and residue screens when switching between apps
    lv_obj_t * black = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(black, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(black, LV_OPA_COVER, 0);
    lv_scr_load(black);
    lv_refr_now(NULL); // Force full flush

    // Then switch to launcher
    launcher_open(&launcher_screen); // launcher handles its own persistence
    lv_scr_load(launcher_screen);
}

// Open an app
// void app_open(AppOpenFunc fn)
// {
//     if(app_screen) {
//         lv_scr_load(app_screen); // Resume existing
//         return;
//     }

//     fn(&app_screen); // App builds its screen
//     lv_scr_load(app_screen);
// }

// void app_open(AppOpenFunc fn)
// {
//     // If same app is already in memory, just show it
//     if(app_screen && fn == current_app_fn) {
//         lv_scr_load(app_screen);
//         return;
//     }

//     // If another app was open, clean it
//     if(app_screen) {
//         lv_obj_del(app_screen);
//         app_screen = NULL;
//     }

//     // Create and load new app
//     fn(&app_screen);
//     lv_scr_load(app_screen);
//     current_app_fn = fn;
// }

void app_open(AppOpenFunc fn)
{
    AppRecord * app = find_or_create_app(fn);
    if(!app) return;

    if(app->screen == NULL) {
        fn(&app->screen); // Build it
    }

    lv_scr_load(app->screen);
    current_app = app;
}

// Minimize app (go back to launcher)
void app_minimize(void)
{
    // STEP 1: Temporary black screen to flush old framebuffer
    lv_obj_t * wipe_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(wipe_screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(wipe_screen, LV_OPA_COVER, 0); // Full opacity

    lv_scr_load(wipe_screen); // Load wipe screen
    lv_refr_now(NULL);        // Force flush

    // STEP 2: Load transparent launcher after wipe
    lv_scr_load(launcher_screen);       // Now load transparent screen
    lv_obj_invalidate(launcher_screen); // Force repaint
    lv_refr_now(NULL);                  // Final flush
    
    if(launcher_screen) {
        lv_scr_load(launcher_screen);
    }
}

// Close app and clear from memory
// void app_close(void)
// {
//     // STEP 1: Temporary black screen to flush old framebuffer
//     lv_obj_t * wipe_screen = lv_obj_create(NULL);
//     lv_obj_set_style_bg_color(wipe_screen, lv_color_black(), 0);
//     lv_obj_set_style_bg_opa(wipe_screen, LV_OPA_COVER, 0); // Full opacity

//     lv_scr_load(wipe_screen); // Load wipe screen
//     lv_refr_now(NULL);        // Force flush

//     // STEP 2: Load transparent launcher after wipe
//     lv_scr_load(launcher_screen);       // Now load transparent screen
//     lv_obj_invalidate(launcher_screen); // Force repaint
//     lv_refr_now(NULL);                  // Final flush

//     // STEP 3: Delete app screen
//     if(app_screen) {
//         lv_obj_del(app_screen);
//         app_screen = NULL;
//     }
// }

void app_close(void)
{
    // STEP 1: Temporary black screen to flush old framebuffer
    lv_obj_t * wipe_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(wipe_screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(wipe_screen, LV_OPA_COVER, 0); // Full opacity

    lv_scr_load(wipe_screen); // Load wipe screen
    lv_refr_now(NULL);        // Force flush

    // STEP 2: Load transparent launcher after wipe
    lv_scr_load(launcher_screen);       // Now load transparent screen
    lv_obj_invalidate(launcher_screen); // Force repaint
    lv_refr_now(NULL);                  // Final flush

    if(current_app && current_app->screen) {
        lv_obj_del(current_app->screen);
        current_app->screen = NULL;
        current_app         = NULL;
    }

    if(launcher_screen) {
        lv_scr_load(launcher_screen);
    }
}
