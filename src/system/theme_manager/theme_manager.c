#define _DEFAULT_SOURCE
#include "theme_manager.h"

#include <unistd.h>
#include <limits.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

struct theme_data_t current_theme;

static lv_color_t parse_hex_color(const char *hex)
{
    if (!hex || strlen(hex) < 6) {
        return lv_color_black();
    }

    // Skip prefix: support #RRGGBB or 0xRRGGBB
    const char *start = hex;
    if (strncmp(hex, "0x", 2) == 0 || strncmp(hex, "#", 1) == 0) {
        start = hex + (hex[0] == '#' ? 1 : 2);
    }

    unsigned int r, g, b;
    sscanf(start, "%02x%02x%02x", &r, &g, &b);

    return lv_color_make(r, g, b);
}

static float parse_alpha(const char *str) {
    float val = atof(str);
    if(val < 0.0f) val = 0.0f;
    if(val > 1.0f) val = 1.0f;
    return val * 255.0f;
}

void theme_manager_load(void)
{
    char realpath[PATH_MAX];
    snprintf(realpath, sizeof(realpath), "./src/settings/theme.json");

    FILE * file = fopen(realpath, "r");
    if(!file) {
        LV_LOG_ERROR("Theme file not found: %s", realpath);
        return;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    rewind(file);

    char * data = malloc(len + 1);
    fread(data, 1, len, file);
    data[len] = '\0';
    fclose(file);

    cJSON * root = cJSON_Parse(data);
    if(!root) {
        LV_LOG_ERROR("Failed to parse theme JSON");
        free(data);
        return;
    }

    cJSON * theme = cJSON_GetObjectItem(root, "theme");
    cJSON * screens = cJSON_GetObjectItem(theme, "screens");
    strncpy(current_theme.name, cJSON_GetObjectItem(theme, "name")->valuestring, sizeof(current_theme.name));

    // --- IDLE SCREEN ---
    cJSON * idle = cJSON_GetObjectItem(screens, "idle");
    cJSON * idle_status_font = cJSON_GetObjectItem(idle, "status-area");
    cJSON * idle_softkey_font = cJSON_GetObjectItem(idle, "softkey-area");
    cJSON * idle_wallpaper = cJSON_GetObjectItem(idle, "wallpaper");

    // Fonts
    cJSON * f1 = cJSON_GetObjectItem(idle_status_font, "font");
    cJSON * f2 = cJSON_GetObjectItem(idle_softkey_font, "font");

    current_theme.idle_screen_status_area_font_color   = parse_hex_color(cJSON_GetObjectItem(f1, "color")->valuestring);
    current_theme.idle_screen_status_area_font_outline = parse_hex_color(cJSON_GetObjectItem(f1, "outline")->valuestring);
    current_theme.idle_screen_status_area_font_size    = cJSON_GetObjectItem(f1, "size")->valueint;

    current_theme.idle_screen_softkey_area_font_color   = parse_hex_color(cJSON_GetObjectItem(f2, "color")->valuestring);
    current_theme.idle_screen_softkey_area_font_outline = parse_hex_color(cJSON_GetObjectItem(f2, "outline")->valuestring);
    current_theme.idle_screen_softkey_area_font_size    = cJSON_GetObjectItem(f2, "size")->valueint;

    cJSON * bg1 = cJSON_GetObjectItem(idle_softkey_font, "background");
    cJSON * wp1 = idle_wallpaper;

    current_theme.idle_screen_softkey_area_bg_color = parse_hex_color(cJSON_GetObjectItem(bg1, "color")->valuestring);
    snprintf(current_theme.idle_screen_softkey_area_bg_source, sizeof(current_theme.idle_screen_softkey_area_bg_source), "A:/themes/default/%s", cJSON_GetObjectItem(bg1, "source")->valuestring);

    current_theme.idle_screen_wallpaper_color = parse_hex_color(cJSON_GetObjectItem(wp1, "color")->valuestring);
    snprintf(current_theme.idle_screen_wallpaper_source, sizeof(current_theme.idle_screen_wallpaper_source), "A:/themes/default/%s", cJSON_GetObjectItem(wp1, "source")->valuestring);
    current_theme.idle_screen_wallpaper_opacity = (short int)parse_alpha(cJSON_GetObjectItem(wp1, "opacity")->valuestring);

    // --- MENU APP ---
    cJSON * menu_app = cJSON_GetObjectItem(screens, "menu-app");
    cJSON * menu_status = cJSON_GetObjectItem(menu_app, "status-area");
    cJSON * menu_softkey = cJSON_GetObjectItem(menu_app, "softkey-area");
    cJSON * menu_wp = cJSON_GetObjectItem(menu_app, "wallpaper");
    cJSON * menu = cJSON_GetObjectItem(menu_app, "menu");
    cJSON * grid = cJSON_GetObjectItem(menu, "grid-menu");
    cJSON * grid_uns = cJSON_GetObjectItem(grid, "unselected");
    cJSON * grid_sel = cJSON_GetObjectItem(grid, "selected");
    cJSON * grid_icon = cJSON_GetObjectItem(grid, "icon-size");

    cJSON * mf1 = cJSON_GetObjectItem(menu_status, "font");
    cJSON * mf2 = cJSON_GetObjectItem(menu_softkey, "font");

    current_theme.menu_app_status_area_font_color   = parse_hex_color(cJSON_GetObjectItem(mf1, "color")->valuestring);
    current_theme.menu_app_status_area_font_outline = parse_hex_color(cJSON_GetObjectItem(mf1, "outline")->valuestring);
    current_theme.menu_app_status_area_font_size    = cJSON_GetObjectItem(mf1, "size")->valueint;

    current_theme.menu_app_softkey_area_font_color   = parse_hex_color(cJSON_GetObjectItem(mf2, "color")->valuestring);
    current_theme.menu_app_softkey_area_font_outline = parse_hex_color(cJSON_GetObjectItem(mf2, "outline")->valuestring);
    current_theme.menu_app_softkey_area_font_size    = cJSON_GetObjectItem(mf2, "size")->valueint;

    cJSON * bg2 = cJSON_GetObjectItem(menu_softkey, "background");
    cJSON * wp2 = menu_wp;

    current_theme.menu_app_softkey_area_bg_color = parse_hex_color(cJSON_GetObjectItem(bg2, "color")->valuestring);
    snprintf(current_theme.menu_app_softkey_area_bg_source, sizeof(current_theme.menu_app_softkey_area_bg_source), "A:/themes/default/%s", cJSON_GetObjectItem(bg2, "source")->valuestring);

    current_theme.menu_app_wallpaper_color = parse_hex_color(cJSON_GetObjectItem(wp2, "color")->valuestring);
    snprintf(current_theme.menu_app_wallpaper_source, sizeof(current_theme.menu_app_wallpaper_source), "A:/themes/default/%s", cJSON_GetObjectItem(wp2, "source")->valuestring);
    current_theme.menu_app_wallpaper_opacity = (short int)parse_alpha(cJSON_GetObjectItem(wp2, "opacity")->valuestring);

    // grid menu
    cJSON * gf1 = cJSON_GetObjectItem(grid_uns, "font");
    cJSON * gf2 = cJSON_GetObjectItem(grid_sel, "font");
    cJSON * gbg = cJSON_GetObjectItem(grid_sel, "background");

    current_theme.menu_app_grid_menu_unselected_font_color = parse_hex_color(cJSON_GetObjectItem(gf1, "color")->valuestring);
    current_theme.menu_app_grid_menu_unselected_font_outline = parse_hex_color(cJSON_GetObjectItem(gf1, "outline")->valuestring);
    current_theme.menu_app_grid_menu_unselected_font_size = cJSON_GetObjectItem(gf1, "size")->valueint;

    current_theme.menu_app_grid_menu_selected_font_color = parse_hex_color(cJSON_GetObjectItem(gf2, "color")->valuestring);
    current_theme.menu_app_grid_menu_selected_font_outline = parse_hex_color(cJSON_GetObjectItem(gf2, "outline")->valuestring);
    current_theme.menu_app_grid_menu_selected_font_size = cJSON_GetObjectItem(gf2, "size")->valueint;

    current_theme.menu_app_grid_menu_selected_bg_color = parse_hex_color(cJSON_GetObjectItem(gbg, "color")->valuestring);
    snprintf(current_theme.menu_app_grid_menu_selected_bg_source, sizeof(current_theme.menu_app_grid_menu_selected_bg_source), "A:/themes/default/%s", cJSON_GetObjectItem(gbg, "source")->valuestring);
    current_theme.menu_app_grid_menu_selected_opacity = (short int)parse_alpha(cJSON_GetObjectItem(gbg, "opacity")->valuestring);

    // icon size
    current_theme.menu_app_grid_menu_icon_width = cJSON_GetObjectItem(grid_icon, "width")->valueint;
    current_theme.menu_app_grid_menu_icon_height = cJSON_GetObjectItem(grid_icon, "height")->valueint;

    cJSON_Delete(root);
    free(data);
}


// // theme_manager.c
// #define _DEFAULT_SOURCE
// #include "theme_manager.h"

// #include <unistd.h> // for getcwd()
// #include <limits.h> // for PATH_MAX

// #include <cjson/cJSON.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>

// #ifndef PATH_MAX
// #define PATH_MAX 4096
// #endif

// struct theme_data_t current_theme;

// // Convert hex color string (e.g., "#FFAA33") to lv_color_t
// static lv_color_t parse_hex_color(const char *hex)
// {
//     if (!hex || strlen(hex) != 7 || hex[0] != '#') return lv_color_black();

//     unsigned int r, g, b;
//     sscanf(hex + 1, "%02x%02x%02x", &r, &g, &b);
//     return lv_color_make(r, g, b);
// }

// static const char * get_string_or_default(cJSON * obj, const char * key, const char * fallback)
// {
//     cJSON * item = cJSON_GetObjectItem(obj, key);
//     if(item && cJSON_IsString(item) && item->valuestring && strlen(item->valuestring) > 0) return item->valuestring;
//     return fallback;
// }

// static lv_color_t get_color_or_default(cJSON * obj, const char * key, const char * fallback_hex)
// {
//     cJSON * item = cJSON_GetObjectItem(obj, key);
//     if(item && cJSON_IsString(item) && strlen(item->valuestring) > 0) return parse_hex_color(item->valuestring);
//     return parse_hex_color(fallback_hex);
// }

// void theme_manager_load(void)
// {
//     char realpath[PATH_MAX];
//     snprintf(realpath, sizeof(realpath), "./src/settings/theme.json");

//     FILE * file = fopen(realpath, "r");
//     if(!file) {
//         LV_LOG_ERROR("Theme file not found: %s", realpath);
//         return;
//     }

//     fseek(file, 0, SEEK_END);
//     long len = ftell(file);
//     rewind(file);

//     char * data = malloc(len + 1);
//     fread(data, 1, len, file);
//     data[len] = '\0';
//     fclose(file);

//     cJSON * root = cJSON_Parse(data);
//     if(!root) {
//         LV_LOG_ERROR("Failed to parse theme JSON");
//         free(data);
//         return;
//     }

//     cJSON * name      = cJSON_GetObjectItem(root, "name");
//     cJSON * wallpaper = cJSON_GetObjectItem(root, "wallpaper");
//     cJSON * icons     = cJSON_GetObjectItem(root, "icons");
//     cJSON * colors    = cJSON_GetObjectItem(root, "colors");

//     strncpy(current_theme.name, get_string_or_default(root, "name", "default"), sizeof(current_theme.name));

//     snprintf(current_theme.wallpaper, sizeof(current_theme.wallpaper), "A:/themes/default/%s",
//              get_string_or_default(root, "wallpaper", "wallpaper.png"));

//     if(colors) {
//         current_theme.bg_color   = get_color_or_default(colors, "bg", "#1E1E1E");
//         current_theme.btn_color  = get_color_or_default(colors, "btn", "#0074D9");
//         current_theme.text_color = get_color_or_default(colors, "text", "#FFFFFF");
//     }

//     // Debug output
//     printf("Theme name: %s\n", current_theme.name);
//     printf("Wallpaper: %s\n", current_theme.wallpaper);
//     // printf("Icon SMS: %s\n", current_theme.icon_sms);
//     // printf("Icon Calculator: %s\n", current_theme.icon_calculator);
//     // printf("Icon Notes: %s\n", current_theme.icon_notes);

//     cJSON_Delete(root);
//     free(data);
// }
