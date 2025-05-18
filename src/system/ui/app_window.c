#include "app_window.h"

#include "lvgl.h"
#include "../app_manager.h" // For app_close() and app_minimize()

void app_close_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    app_close();
}

void app_minimize_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    app_minimize();
}

AppWindow app_window_create(const char * title_text)
{
    AppWindow win;

    // Base screen — no flex layout
    win.screen = lv_obj_create(NULL);
    lv_obj_set_size(win.screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(win.screen, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);
    lv_obj_clear_flag(win.screen, LV_OBJ_FLAG_SCROLLABLE);

    // Fixed height top bar (30px)
    lv_obj_t * top_bar = lv_obj_create(win.screen);
    lv_obj_set_size(top_bar, LV_HOR_RES, 30);
    lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 0);  // Align at top
    lv_obj_set_style_bg_color(top_bar, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
    lv_obj_set_style_pad_all(top_bar, 5, 0);
    lv_obj_set_style_radius(top_bar, 0, 0);
    lv_obj_set_style_border_side(top_bar, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_width(top_bar, 1, 0);
    lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);

    // Title label
    lv_obj_t * title = lv_label_create(top_bar);
    lv_label_set_text(title, title_text);
    lv_obj_center(title);

    // Close button
    lv_obj_t * close_btn = lv_btn_create(top_bar);
    lv_obj_set_size(close_btn, 18, 18);
    lv_obj_align(close_btn, LV_ALIGN_RIGHT_MID, -3, 0);
    lv_obj_set_style_radius(close_btn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "X");
    lv_obj_set_style_text_font(close_label, &lv_font_montserrat_10, 0);
    lv_obj_center(close_label);
    lv_obj_add_event_cb(close_btn, app_close_cb, LV_EVENT_CLICKED, NULL);

    // Minimize button
    lv_obj_t * minimize_btn = lv_btn_create(top_bar);
    lv_obj_set_size(minimize_btn, 18, 18);
    lv_obj_align(minimize_btn, LV_ALIGN_RIGHT_MID, -25, 0);

    lv_obj_t * minimize_label = lv_label_create(minimize_btn);
    lv_label_set_text(minimize_label, "_");
    lv_obj_set_style_text_font(minimize_label, &lv_font_montserrat_10, 0);
    lv_obj_center(minimize_label);
    lv_obj_add_event_cb(minimize_btn, app_minimize_cb, LV_EVENT_CLICKED, NULL);

    // Content container: positioned below top_bar
    win.content = lv_obj_create(win.screen);
    lv_obj_set_size(win.content, LV_HOR_RES, LV_VER_RES - 30);  // screen height - top bar
    lv_obj_align(win.content, LV_ALIGN_BOTTOM_MID, 0, 0);       // Bottom-aligned with screen
    lv_obj_set_style_pad_all(win.content, 10, 0);
    lv_obj_set_style_radius(win.content, 0, 0);
    lv_obj_set_style_bg_opa(win.content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(win.content, 0, 0);
    lv_obj_clear_flag(win.content, LV_OBJ_FLAG_SCROLLABLE);

    return win;
}


// flex layout
// AppWindow app_window_create(const char * title_text)
// {
//     AppWindow win;

//     // Create full-screen base object and apply vertical flex layout
//     // This ensures that the top bar and content stack vertically
//     win.screen = lv_obj_create(NULL);
//     lv_obj_set_flex_flow(win.screen, LV_FLEX_FLOW_COLUMN); // Stack children vertically
//     lv_obj_set_style_bg_color(win.screen, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);
//     lv_obj_clear_flag(win.screen, LV_OBJ_FLAG_SCROLLABLE);
//     lv_obj_set_size(win.screen, LV_HOR_RES, LV_VER_RES); // Fill screen resolution
//     lv_obj_set_style_pad_all(win.screen, 0, 0);
//     lv_obj_set_style_pad_gap(win.screen, 0, 0); // No gap between flex items

//     // ---------- Top Bar (Title + Minimize/Close) ----------

//     // Create a fixed-height top bar (acts like a window title bar)
//     lv_obj_t * top_bar = lv_obj_create(win.screen);
//     lv_obj_set_size(top_bar, LV_PCT(100), 30); // Full width, fixed height
//     lv_obj_set_style_bg_color(top_bar, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
//     lv_obj_set_style_pad_all(top_bar, 5, 0);
//     lv_obj_set_style_radius(top_bar, 0, 0);
//     lv_obj_set_style_border_side(top_bar, LV_BORDER_SIDE_BOTTOM, 0); // Only bottom border
//     lv_obj_set_style_border_width(top_bar, 1, 0);                    // Thin border line
//     lv_obj_clear_flag(top_bar, LV_OBJ_FLAG_SCROLLABLE);

//     // Title text centered (can be aligned left if desired)
//     lv_obj_t * title = lv_label_create(top_bar);
//     lv_label_set_text(title, title_text);
//     lv_obj_center(title);
//     // Alternative alignment if needed: lv_obj_align(title, LV_ALIGN_LEFT_MID, 10, 0);

//     // Close button on right
//     lv_obj_t * close_btn = lv_btn_create(top_bar);
//     lv_obj_set_size(close_btn, 18, 18);
//     lv_obj_align(close_btn, LV_ALIGN_RIGHT_MID, -3, 0);
//     lv_obj_set_style_radius(close_btn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    
//     lv_obj_t * close_label = lv_label_create(close_btn);
//     lv_label_set_text(close_label, "X");
//     lv_obj_set_style_text_font(close_label, &lv_font_montserrat_10, 0);
//     lv_obj_center(close_label); // Center label inside button
//     lv_obj_add_event_cb(close_btn, app_close_cb, LV_EVENT_CLICKED, NULL);

//     // Minimize button next to close button
//     lv_obj_t * minimize_btn = lv_btn_create(top_bar);
//     lv_obj_set_size(minimize_btn, 18, 18);
//     lv_obj_align(minimize_btn, LV_ALIGN_RIGHT_MID, -25, 0); // Positioned left of close
//     lv_obj_set_style_radius(minimize_btn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    
//     lv_obj_t * minimize_label = lv_label_create(minimize_btn);
//     lv_label_set_text(minimize_label, "_");
//     lv_obj_set_style_text_font(minimize_label, &lv_font_montserrat_10, 0);
//     lv_obj_center(minimize_label);
//     lv_obj_add_event_cb(minimize_btn, app_minimize_cb, LV_EVENT_CLICKED, NULL);

//     // ---------- Content Area ----------

//     // Create main app content container that fills remaining space
//     // NOTE: Do NOT apply flex here — app developer controls layout inside this
//     win.content = lv_obj_create(win.screen);
//     lv_obj_set_size(win.content, LV_PCT(100), LV_PCT(100)); // Full width, flexible height
//     lv_obj_set_flex_grow(win.content, 1); // Take up remaining vertical space
//     lv_obj_set_style_pad_all(win.content, 10, 0); // Default inner padding
//     lv_obj_set_style_radius(win.content, 0, 0);
//     lv_obj_set_style_bg_opa(win.content, LV_OPA_TRANSP, 0); // Transparent background
//     lv_obj_set_style_border_width(win.content, 0, 0); // No border
//     lv_obj_clear_flag(win.content, LV_OBJ_FLAG_SCROLLABLE);

//     // ⚠️ Important: do NOT set flex layout inside win.content.
//     // Let each app decide how it wants to lay out its UI.

//     return win;
// }

