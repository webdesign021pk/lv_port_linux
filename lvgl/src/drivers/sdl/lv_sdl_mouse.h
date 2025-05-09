/**
 * @file lv_sdl_mouse.h
 *
 */

#ifndef LV_SDL_MOUSE_H
#define LV_SDL_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_window.h"
#if LV_USE_SDL

// added this for lv_sdl_mouse_handler
#include <SDL2/SDL.h>  // add this if not already included


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_indev_t * lv_sdl_mouse_create(void);

void lv_sdl_mouse_handler(SDL_Event * event);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_MOUSE_H */
