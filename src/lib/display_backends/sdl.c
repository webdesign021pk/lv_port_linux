/**
 * @file sdl.c
 *
 * The backend for the SDL simulator
 *
 * Based on the original file from the repository
 *
 * - Move to a separate file
 *   2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lvgl/lvgl.h"
#if LV_USE_SDL

#include <SDL2/SDL.h>               // ✅ ADD THIS LINE HERE

#include "../simulator_util.h"
#include "../simulator_settings.h"
#include "../backends.h"

// keyboard and mouse
#include "../../../lvgl/src/drivers/sdl/lv_sdl_keyboard.h"
#include "../../../lvgl/src/drivers/sdl/lv_sdl_mouse.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  EXTERNAL VARIABLES
 **********************/
extern simulator_settings_t settings;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void run_loop_sdl(void);
static lv_display_t *init_sdl(void);

/**********************
 *  STATIC VARIABLES
 **********************/

static char *backend_name = "SDL";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register the backend
 * @param backend the backend descriptor
 * @description configures the descriptor
 */
int backend_init_sdl(backend_t *backend)
{
    LV_ASSERT_NULL(backend);

    backend->handle->display = malloc(sizeof(display_backend_t));
    LV_ASSERT_NULL(backend->handle->display);

    backend->handle->display->init_display = init_sdl;
    backend->handle->display->run_loop = run_loop_sdl;
    backend->name = backend_name;
    backend->type = BACKEND_DISPLAY;

    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the SDL display driver
 *
 * @return the LVGL display
 */
static lv_display_t *init_sdl(void)
{
    lv_display_t *disp;

    disp = lv_sdl_window_create(settings.window_width, settings.window_height);

    if (disp == NULL) {
        return NULL;
    }

    return disp;
}

/**
 * The run loop of the SDL driver
 */
// static void run_loop_sdl(void)
// {
//     uint32_t idle_time;

//     /* Handle LVGL tasks */
//     while (true) {
//         /* Returns the time to the next timer execution */
//         idle_time = lv_timer_handler();
//         usleep(idle_time * 1000);
//     }
// }

static void run_loop_sdl(void)
{
    SDL_Event event;

    // SDL_StartTextInput();  // 🔑 Enables text input from SDL
    if (!SDL_IsTextInputActive()) {
        SDL_StartTextInput();
        printf("✅ SDL Text Input started\n");
    }

    while (true) {
        // 🔁 Handle all pending SDL events
        while (SDL_PollEvent(&event)) {
            // 👇 Handle window resize
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_RESIZED) {

                int new_w = event.window.data1;
                int new_h = event.window.data2;

                printf("🔁 Window resized to %dx%d\n", new_w, new_h);

                // Update LVGL's internal resolution
                lv_display_set_resolution(lv_display_get_default(), new_w, new_h);
            }
            
            // printf("SDL Event type: %d\n", event.type);

            if (event.type == SDL_KEYDOWN) {
                printf("KEYDOWN: %s\n", SDL_GetKeyName(event.key.keysym.sym));
            }
            if (event.type == SDL_TEXTINPUT) {
                printf("TEXTINPUT: %s\n", event.text.text);
            }
        
            lv_sdl_keyboard_handler(&event);  // ⌨️ Send key input to LVGL
            lv_sdl_mouse_handler(&event);     // 🖱️ Send mouse events to LVGL

            // Optional: Exit on ESC or window close
            if (event.type == SDL_QUIT ||
               (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                SDL_Quit();
                exit(0);
            }
        }

        // 🔁 Let LVGL do its rendering and internal tasks
        lv_timer_handler();

        // 💤 Prevent CPU from maxing out
        SDL_Delay(5);
    }
}

#endif /*#if LV_USE_SDL*/
