// theme_manager.c
#define _DEFAULT_SOURCE
#include "theme_manager.h"

#include <unistd.h> // for getcwd()
#include <limits.h> // for PATH_MAX

#include <cjson/cJSON.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

struct theme_data_t current_theme;

// Convert hex color string (e.g., "#FFAA33") to lv_color_t
static lv_color_t parse_hex_color(const char *hex)
{
    if (!hex || strlen(hex) != 7 || hex[0] != '#') return lv_color_black();

    unsigned int r, g, b;
    sscanf(hex + 1, "%02x%02x%02x", &r, &g, &b);
    return lv_color_make(r, g, b);
}

static const char * get_string_or_default(cJSON * obj, const char * key, const char * fallback)
{
    cJSON * item = cJSON_GetObjectItem(obj, key);
    if(item && cJSON_IsString(item) && item->valuestring && strlen(item->valuestring) > 0) return item->valuestring;
    return fallback;
}

static lv_color_t get_color_or_default(cJSON * obj, const char * key, const char * fallback_hex)
{
    cJSON * item = cJSON_GetObjectItem(obj, key);
    if(item && cJSON_IsString(item) && strlen(item->valuestring) > 0) return parse_hex_color(item->valuestring);
    return parse_hex_color(fallback_hex);
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

    cJSON * name      = cJSON_GetObjectItem(root, "name");
    cJSON * wallpaper = cJSON_GetObjectItem(root, "wallpaper");
    cJSON * icons     = cJSON_GetObjectItem(root, "icons");
    cJSON * colors    = cJSON_GetObjectItem(root, "colors");

    strncpy(current_theme.name, get_string_or_default(root, "name", "default"), sizeof(current_theme.name));

    snprintf(current_theme.wallpaper, sizeof(current_theme.wallpaper), "A:/themes/default/%s",
             get_string_or_default(root, "wallpaper", "wallpaper.png"));

    if(colors) {
        current_theme.bg_color   = get_color_or_default(colors, "bg", "#1E1E1E");
        current_theme.btn_color  = get_color_or_default(colors, "btn", "#0074D9");
        current_theme.text_color = get_color_or_default(colors, "text", "#FFFFFF");
    }

    // Debug output
    printf("Theme name: %s\n", current_theme.name);
    printf("Wallpaper: %s\n", current_theme.wallpaper);
    // printf("Icon SMS: %s\n", current_theme.icon_sms);
    // printf("Icon Calculator: %s\n", current_theme.icon_calculator);
    // printf("Icon Notes: %s\n", current_theme.icon_notes);

    cJSON_Delete(root);
    free(data);
}
