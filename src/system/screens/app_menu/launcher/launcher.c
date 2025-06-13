#include "launcher.h"
#include "../system/app_manager.h"
#include "../system/theme_manager/theme_manager.h"
#include "../system/app_registry/app_registry.h"
#include "../system/screens/idle/home_screen/home_screen.h"
#include "lvgl.h"
#include <stdio.h>
#include <string.h>

#include <unistd.h> // for fork(), execl(), _exit()
#include <stdlib.h> // for NULL
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/stat.h>


/** * === native app structure start === * **/
typedef struct
{
    char name[128];
    char exec[256];
    char icon[256];
} NativeAppInfo;

#define MAX_NATIVE_APPS 132
static NativeAppInfo native_apps[MAX_NATIVE_APPS];
static int native_app_count = 0;

const char * icon_dirs[] = {
    "/usr/share/icons/hicolor/48x48/apps",    "/usr/share/icons/hicolor/64x64/apps", "/usr/share/pixmaps",
    "/usr/share/icons/hicolor/scalable/apps", // for SVGs
    "/usr/share/icons/Adwaita/48x48/apps",
};
#define ICON_CACHE_FMT "%s/.cache/launcher/icons/hicolor/48x48/apps"
#define FALLBACK_ICON  "A:/settings/theme/icon_default.png"
/** * === native app structure end === * **/


// Nokia N73 style constants
#define ICON_WIDTH 42
#define ICON_HEIGHT 42
#define LABEL_HEIGHT 16
#define ITEM_PADDING 8
#define GRID_PADDING 12
#define MAX_LABEL_LENGTH 12 // Characters before truncation

// Static variables
static lv_obj_t * launcher_screen_internal = NULL;
static lv_obj_t * home_screen              = NULL;

// Function prototypes
static void menu_btn_cb(lv_event_t * e);
static void open_app_cb(lv_event_t * e);
static void create_menu_button(lv_obj_t * parent);
static void create_app_grid(lv_obj_t * parent);
static void create_app_item(lv_obj_t * container, int app_index);
static void create_native_app_item(lv_obj_t * container, int app_index);

static const char * truncate_label(const char * original);

void open_app_cb(lv_event_t * e)
{
    AppOpenFunc open_fn = (AppOpenFunc)lv_event_get_user_data(e);
    if(open_fn) {
        app_open(open_fn);
    }
}

// native app functions start

bool file_exists(const char * path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

void resolve_icon_path(const char *icon_name, char *out_path, size_t out_size)
{
    const char *png_sources[] = {
        "/usr/share/icons/hicolor/128x128/apps",
        "/usr/share/icons/hicolor/64x64/apps",
        "/usr/share/icons/hicolor/48x48/apps",
        "/usr/share/pixmaps",
        "/usr/share/icons/HighContrast/48x48/apps",
        NULL
    };

    const char *svg_sources[] = {
        "/usr/share/icons/hicolor/scalable/apps",
        "/usr/share/icons/Humanity/apps/48",
        NULL
    };

    // Resolve cache path
    char cache_folder[512];
    snprintf(cache_folder, sizeof(cache_folder), ICON_CACHE_FMT, getenv("HOME"));

    // Ensure cache directory exists (main() should also do this)
    char mkdir_cmd[768];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p '%s'", cache_folder);
    system(mkdir_cmd);

    // Path to cached PNG
    char cached_png[512];
    snprintf(cached_png, sizeof(cached_png), "%s/%s.png", cache_folder, icon_name);

    // Step 1: Use cached PNG
    if (access(cached_png, R_OK) == 0) {
        snprintf(out_path, out_size, "S:%s", cached_png);
        return;
    }

    // Step 2: Try system .png locations
    for (int i = 0; png_sources[i]; i++) {
        char system_png[512];
        snprintf(system_png, sizeof(system_png), "%s/%s.png", png_sources[i], icon_name);
        if (access(system_png, R_OK) == 0) {
            char copy_cmd[1024];
            snprintf(copy_cmd, sizeof(copy_cmd), "cp '%s' '%s'", system_png, cached_png);
            system(copy_cmd);
            if (access(cached_png, R_OK) == 0) {
                snprintf(out_path, out_size, "S:%s", cached_png);
                return;
            }
        }
    }

    // Step 3: Try SVG sources and convert
    for (int i = 0; svg_sources[i]; i++) {
        char svg_path[512];
        snprintf(svg_path, sizeof(svg_path), "%s/%s.svg", svg_sources[i], icon_name);
        if (access(svg_path, R_OK) == 0) {
            char convert_cmd[1024];
            snprintf(convert_cmd, sizeof(convert_cmd), "rsvg-convert -w 48 -h 48 -o '%s' '%s'", cached_png, svg_path);
            system(convert_cmd);
            if (access(cached_png, R_OK) == 0) {
                snprintf(out_path, out_size, "S:%s", cached_png);
                return;
            }
        }
    }

    // Final fallback
    snprintf(out_path, out_size, FALLBACK_ICON);
}

static void parse_desktop_files()
{
    native_app_count = 0;

    DIR * dir = opendir("/usr/share/applications");
    if(!dir) {
        perror("Could not open /usr/share/applications");
        return;
    }

    struct dirent * entry;
    while((entry = readdir(dir)) && native_app_count < MAX_NATIVE_APPS) {
        if(!strstr(entry->d_name, ".desktop")) continue;

        char filepath[256];
        snprintf(filepath, sizeof(filepath), "/usr/share/applications/%s", entry->d_name);

        FILE * fp = fopen(filepath, "r");
        if(!fp) continue;

        NativeAppInfo app = {0};
        char line[512];
        bool skip             = false;
        bool in_desktop_entry = false;

        while(fgets(line, sizeof(line), fp)) {
            // Remove trailing newline
            line[strcspn(line, "\n")] = '\0';

            // Skip empty lines and comments
            if(line[0] == '\0' || line[0] == '#') continue;

            // Check for section headers
            if(line[0] == '[') {
                if(strcmp(line, "[Desktop Entry]") == 0) {
                    in_desktop_entry = true;
                } else {
                    in_desktop_entry = false;
                }
                continue;
            }

            // Only process if we're in the [Desktop Entry] section
            if(!in_desktop_entry) continue;

            // Parse key-value pairs
            if(strncmp(line, "Name=", 5) == 0) {
                strncpy(app.name, line + 5, sizeof(app.name) - 1);
            } else if(strncmp(line, "Exec=", 5) == 0) {
                strncpy(app.exec, line + 5, sizeof(app.exec) - 1);
            } else if(strncmp(line, "Icon=", 5) == 0) {
                strncpy(app.icon, line + 5, sizeof(app.icon) - 1);
                resolve_icon_path(app.icon, app.icon, sizeof(app.icon));
            } else if(strncmp(line, "NoDisplay=", 10) == 0) {
                if(strcmp(line + 10, "true") == 0) skip = true;
            } else if(strncmp(line, "Hidden=", 7) == 0) {
                if(strcmp(line + 7, "true") == 0) skip = true;
            } else if(strncmp(line, "Terminal=", 9) == 0) {
                if(strcmp(line + 9, "true") == 0) skip = true;
            }
        }
        fclose(fp);

        // Skip if marked as NoDisplay/Hidden or missing required fields
        if(skip || strlen(app.name) == 0 || strlen(app.exec) == 0) continue;

        // Clean command: remove arguments and unescape characters
        char * space = strchr(app.exec, ' ');
        if(space) *space = '\0';

        // Remove potential % arguments
        char * percent = strchr(app.exec, '%');
        if(percent) *percent = '\0';

        // Only add if we have valid name and executable
        if(strlen(app.name) > 0 && strlen(app.exec) > 0) {
            native_apps[native_app_count++] = app;
        }
    }

    closedir(dir);
}


void launch_native_app(lv_event_t * e)
{
    const char * cmd = lv_event_get_user_data(e);
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "DISPLAY=:0 %s &", cmd);
    printf("Launching native: %s\n", buffer);
    system(buffer);
}

static void create_native_app_buttons(lv_obj_t * container)
{
    parse_desktop_files();

    for(int i = 0; i < native_app_count; i++) {
        create_native_app_item(container, i);
    }
}

static void create_native_app_item(lv_obj_t * container, int app_index)
{
    NativeAppInfo * app = &native_apps[app_index];

    // variables
    uint8_t btn_w = current_theme.menu_app_grid_menu_icon_width;
    uint8_t btn_h = current_theme.menu_app_grid_menu_icon_height;

    // Create button container
    lv_obj_t * btn = lv_btn_create(container);
    lv_obj_set_size(btn, btn_w + ITEM_PADDING, btn_h + LABEL_HEIGHT + ITEM_PADDING);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_style_radius(btn, 4, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_white(), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);

    // Add pressed effect
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(btn, LV_OPA_20, LV_STATE_PRESSED);

    // Create icon
    lv_obj_t * img             = lv_img_create(btn);
    const char * icon_path     = app->icon; // Use the icon path from the app info
    const char * fallback_path = "A:/settings/theme/icon_default.png";

    // Check if icon exists
    lv_fs_file_t f;
    if(lv_fs_open(&f, icon_path, LV_FS_MODE_RD) == LV_FS_RES_OK) {
        lv_fs_close(&f);
        lv_img_set_src(img, icon_path);
    } else {
        lv_img_set_src(img, fallback_path);
    }

    // Get image source descriptor
    const lv_image_dsc_t * dsc = lv_image_get_src(img);

    // Get original image size
    int16_t src_w = lv_image_get_src_width(img);
    int16_t src_h = lv_image_get_src_height(img);

    // Only proceed if image has valid dimensions
    if(src_w > 0 && src_h > 0) {
        // Default zoom: 100% (LVGL uses 256 for 1x scale)
        uint16_t zoom = 256;

        // If image is larger than max size, calculate zoom
        if(src_w > btn_w || src_h > btn_h) {
            // Calculate scale factors using fixed-point math (256 = 100%)
            uint16_t zoom_w = (btn_w * 255 / src_w);
            uint16_t zoom_h = (btn_h * 255 / src_h);
            zoom            = LV_MIN(zoom_w, zoom_h); // Pick smaller zoom to fit both W and H
            // Apply zoom to image
            lv_image_set_scale(img, zoom);
        }
    }

    // Set icon size and position
    lv_obj_set_size(img, ICON_WIDTH, ICON_HEIGHT);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);

    // Create label
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, app->name);
    lv_obj_set_width(label, ICON_WIDTH);

    // this will enable N73-style truncation
    lv_obj_set_height(label, LABEL_HEIGHT);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT); // N73-style truncation

    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, ICON_HEIGHT);

    // Label styling
    lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(label, current_theme.menu_app_grid_menu_selected_font_color, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    // Callback

    lv_obj_add_event_cb(btn, launch_native_app, LV_EVENT_CLICKED, app->exec);

}

/** * === native app functions end === * **/ 


static void menu_btn_cb(lv_event_t * e)
{
    lv_scr_load(home_screen_create());
}

void launcher_open(lv_obj_t ** out_launcher_screen)
{
    if(launcher_screen_internal) {
        *out_launcher_screen = launcher_screen_internal;
        return;
    }

    // Create screen
    lv_obj_t * scr           = lv_obj_create(NULL);
    launcher_screen_internal = scr;

    // Set background style
    lv_obj_set_style_bg_color(scr, current_theme.menu_app_wallpaper_color, 0);
    lv_obj_set_style_bg_opa(scr, current_theme.menu_app_wallpaper_opacity, 0);

    // Create title
    lv_obj_t * title = lv_label_create(scr);
    lv_label_set_text(title, "Menu");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Create app grid
    create_app_grid(scr);

    // Create menu button
    create_menu_button(scr);

    // Pass back the pointer
    *out_launcher_screen = scr;
}

static void create_app_grid(lv_obj_t * parent)
{
    // Load apps from registry
    // build_app_registry("src/apps");
    build_app_registry("src/apps");

    // Create container
    lv_obj_t * container = lv_obj_create(parent);
    lv_obj_set_size(container, LV_PCT(90), LV_PCT(75));

    // Grid layout settings
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 40); // Slightly lower for aesthetics

    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP); // Enables row-wrapping
    lv_obj_set_flex_align(container,
                          LV_FLEX_ALIGN_CENTER, // center icons horizontally in row
                          LV_FLEX_ALIGN_CENTER, // center icons horizontally in row
                          LV_FLEX_ALIGN_START   // top icons vertically
    );

    // Padding and styling
    lv_obj_set_style_pad_all(container, GRID_PADDING, 0);
    lv_obj_set_style_pad_row(container, GRID_PADDING, 0);
    lv_obj_set_style_pad_column(container, GRID_PADDING, 0);
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);

    // Create app items
    for(int i = 0; i < app_count; i++) {
        create_app_item(container, i);
    }
    // create_native_app_item(container, 0); // Add native app item
    create_native_app_buttons(container);
}

static void create_app_item(lv_obj_t * container, int app_index)
{
    // variables
    uint8_t btn_w = current_theme.menu_app_grid_menu_icon_width;
    uint8_t btn_h = current_theme.menu_app_grid_menu_icon_height;

    // Create button container
    lv_obj_t * btn = lv_btn_create(container);
    lv_obj_set_size(btn, btn_w + ITEM_PADDING, btn_h + LABEL_HEIGHT + ITEM_PADDING);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_style_radius(btn, 4, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_white(), 0);
    lv_obj_set_style_shadow_opa(btn, LV_OPA_TRANSP, 0);

    // Add pressed effect
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(btn, LV_OPA_20, LV_STATE_PRESSED);

    // Create icon
    lv_obj_t * img             = lv_img_create(btn);
    const char * icon_path     = app_registry[app_index].icon_path;
    const char * fallback_path = "A:/settings/theme/icon_default.png";

    // Check if icon exists
    lv_fs_file_t f;
    if(lv_fs_open(&f, icon_path, LV_FS_MODE_RD) == LV_FS_RES_OK) {
        lv_fs_close(&f);
        lv_img_set_src(img, icon_path);
    } else {
        lv_img_set_src(img, fallback_path);
    }

    // Get image source descriptor
    const lv_image_dsc_t * dsc = lv_image_get_src(img);

    // Get original image size
    int16_t src_w = lv_image_get_src_width(img);
    int16_t src_h = lv_image_get_src_height(img);

    // Only proceed if image has valid dimensions
    if(src_w > 0 && src_h > 0) {
        // Default zoom: 100% (LVGL uses 256 for 1x scale)
        uint16_t zoom = 256;

        // If image is larger than max size, calculate zoom
        if(src_w > btn_w || src_h > btn_h) {
            // Calculate scale factors using fixed-point math (256 = 100%)
            uint16_t zoom_w = (btn_w * 255 / src_w);
            uint16_t zoom_h = (btn_h * 255 / src_h);
            zoom            = LV_MIN(zoom_w, zoom_h); // Pick smaller zoom to fit both W and H
            // Apply zoom to image
            lv_image_set_scale(img, zoom);
        }
    }

    // Set icon size and position
    lv_obj_set_size(img, ICON_WIDTH, ICON_HEIGHT);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);

    // Create label
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, app_registry[app_index].name);
    lv_obj_set_width(label, ICON_WIDTH);

    // this will enable N73-style truncation
    lv_obj_set_height(label, LABEL_HEIGHT);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT); // N73-style truncation

    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, ICON_HEIGHT);

    // Label styling
    lv_obj_set_style_text_font(label, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(label, current_theme.menu_app_grid_menu_selected_font_color, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

    // Add click event
    lv_obj_add_event_cb(btn, open_app_cb, LV_EVENT_CLICKED, app_registry[app_index].exec_fn);
}

static void create_menu_button(lv_obj_t * parent)
{
    // Create button
    lv_obj_t * menu_btn = lv_btn_create(parent);
    lv_obj_align(menu_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_size(menu_btn, 80, 24);
    lv_obj_set_style_radius(menu_btn, 12, 0);

    // Add pressed effect
    lv_obj_set_style_bg_color(menu_btn, lv_color_white(), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(menu_btn, LV_OPA_20, LV_STATE_PRESSED);

    // Create label
    lv_obj_t * btn_label = lv_label_create(menu_btn);
    lv_label_set_text(btn_label, "Home");
    lv_obj_align(btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(btn_label, current_theme.menu_app_grid_menu_selected_font_color, 0);

    // Add click event
    lv_obj_add_event_cb(menu_btn, menu_btn_cb, LV_EVENT_CLICKED, NULL);
}

static const char * truncate_label(const char * original)
{
    static char truncated[MAX_LABEL_LENGTH + 4]; // +4 for ellipsis and null terminator
    size_t len = strlen(original);

    if(len <= MAX_LABEL_LENGTH) {
        return original;
    }

    strncpy(truncated, original, MAX_LABEL_LENGTH);
    truncated[MAX_LABEL_LENGTH] = '\0';
    strcat(truncated, "...");
    return truncated;
}

