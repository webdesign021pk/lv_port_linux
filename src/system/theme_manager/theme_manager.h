// theme_manager.h
#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void theme_manager_load(void);
void theme_manager_apply(const char * theme_name);
void theme_manager_save(void);

extern struct theme_data_t {
    // Basic
    char name[32];

    // idle screen status area
    lv_color_t idle_screen_status_area_font_color;
    lv_color_t idle_screen_status_area_font_outline;
    uint8_t idle_screen_status_area_font_size;
    
    // idle screen softkey area
    lv_color_t idle_screen_softkey_area_font_color;
    lv_color_t idle_screen_softkey_area_font_outline;
    uint8_t idle_screen_softkey_area_font_size;
    lv_color_t idle_screen_softkey_area_bg_color;
    char idle_screen_softkey_area_bg_source[128];
    
    // idle screen wallpaper
    char idle_screen_wallpaper_source[128];
    lv_color_t idle_screen_wallpaper_color;
    uint8_t idle_screen_wallpaper_opacity;  // 0-255

    // menu app status area
    lv_color_t menu_app_status_area_font_color;
    lv_color_t menu_app_status_area_font_outline;
    uint8_t menu_app_status_area_font_size;
    
    // menu app softkey area
    lv_color_t menu_app_softkey_area_font_color;
    lv_color_t menu_app_softkey_area_font_outline;
    uint8_t menu_app_softkey_area_font_size;
    lv_color_t menu_app_softkey_area_bg_color;
    char menu_app_softkey_area_bg_source[128];
    
    // menu app wallpaper
    char menu_app_wallpaper_source[128];
    lv_color_t menu_app_wallpaper_color;
    uint8_t menu_app_wallpaper_opacity;  // 0-255

    // menu app grid menu unselected
    lv_color_t menu_app_grid_menu_unselected_font_color;
    lv_color_t menu_app_grid_menu_unselected_font_outline;
    uint8_t menu_app_grid_menu_unselected_font_size;

    // menu app grid menu selected
    lv_color_t menu_app_grid_menu_selected_font_color;
    lv_color_t menu_app_grid_menu_selected_font_outline;
    uint8_t menu_app_grid_menu_selected_font_size;
    lv_color_t menu_app_grid_menu_selected_bg_color;
    char menu_app_grid_menu_selected_bg_source[128];
    uint8_t menu_app_grid_menu_selected_opacity;  // 0-255
    
    // menu app grid menu icon 
    uint8_t menu_app_grid_menu_icon_width;
    uint8_t menu_app_grid_menu_icon_height;
    
    // Future fields (placeholders)
    // int grid_rows;
    // int grid_cols;
} current_theme;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // THEME_MANAGER_H
