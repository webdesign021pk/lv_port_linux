#include "calculator.h"
#include "system/ui/app_window.h"
#include "lvgl.h"
#include <stdio.h>

// Calculator state
// expression state
static lv_obj_t * g_expr_label = NULL; // New expression label
static char expr_text[64]      = "";
// display state
static char display_text[32]   = "0";
static double current_value    = 0;
static char last_operation     = 0;
static bool new_input_expected = true; // Add this flag

// Update the display
static void update_display(lv_obj_t * display_label)
{
    lv_label_set_text(display_label, display_text);
}

// Handle button clicks
static void button_cb(lv_event_t * e)
{
    lv_obj_t * display_label = (lv_obj_t *)lv_event_get_user_data(e);
    const char * btn_text    = lv_label_get_text(lv_obj_get_child(lv_event_get_target(e), 0));
    char btn_char            = btn_text[0];

    // Handle digits
    if(btn_char >= '0' && btn_char <= '9') {
        if(new_input_expected || strcmp(display_text, "0") == 0) {
            snprintf(display_text, sizeof(display_text), "%c", btn_char);
            new_input_expected = false;
        } else {
            strncat(display_text, btn_text, sizeof(display_text) - strlen(display_text) - 1);
        }
        strncat(expr_text, btn_text, sizeof(expr_text) - strlen(expr_text) - 1);

    }
    // Handle decimal point
    else if(btn_char == '.') {
        if(new_input_expected) {
            snprintf(display_text, sizeof(display_text), "0.");
            new_input_expected = false;
        } else if(strchr(display_text, '.') == NULL) {
            strncat(display_text, ".", sizeof(display_text) - strlen(display_text) - 1);
        }
        strncat(expr_text, btn_text, sizeof(expr_text) - strlen(expr_text) - 1);

    }
    // Handle operations
    else if(btn_char == '+' || btn_char == '-' || btn_char == '*' || btn_char == '/') {
        double entered_value = atof(display_text);
        if(!new_input_expected) {
            strncat(expr_text, btn_text, sizeof(expr_text) - strlen(expr_text) - 1);
        }
        if(last_operation && !new_input_expected) {
            switch(last_operation) {
                case '+': current_value += entered_value; break;
                case '-': current_value -= entered_value; break;
                case '*': current_value *= entered_value; break;
                case '/': current_value /= entered_value; break;
            }
            snprintf(display_text, sizeof(display_text), "%.6g", current_value);
            update_display(display_label);
        } else if(!new_input_expected) {
            current_value = entered_value;
        }

        // ✅ Prevent duplicate operators
        size_t len = strlen(expr_text);
        if(len > 0) {
            char last = expr_text[len - 1];
            if(last == '+' || last == '-' || last == '*' || last == '/') {
                expr_text[len - 1] = btn_char; // Replace the last operator
            } else {
                strncat(expr_text, btn_text, sizeof(expr_text) - len - 1); // Append
            }
        } else {
            strncat(expr_text, btn_text, sizeof(expr_text) - len - 1);
        }

        if(g_expr_label) lv_label_set_text(g_expr_label, expr_text);

        last_operation     = btn_char;
        new_input_expected = true;
    }
    // Handle equals
    else if(btn_char == '=') {
        if(last_operation && !new_input_expected) {
            double entered_value = atof(display_text);

            switch(last_operation) {
                case '+': current_value += entered_value; break;
                case '-': current_value -= entered_value; break;
                case '*': current_value *= entered_value; break;
                case '/': current_value /= entered_value; break;
            }

            snprintf(display_text, sizeof(display_text), "%.6g", current_value);

            // ✅ Show result in expression too
            snprintf(expr_text, sizeof(expr_text), "%.6g", current_value);
            if(g_expr_label) lv_label_set_text(g_expr_label, expr_text);

            last_operation     = 0;
            new_input_expected = true;
        }
    }

    // Handle clear
    else if(strcmp(btn_text, "C") == 0) {
        current_value  = 0;
        last_operation = 0;
        strcpy(display_text, "0");
        new_input_expected = true;
        expr_text[0]       = '\0'; // Reset expression line
    }
    if(g_expr_label) lv_label_set_text(g_expr_label, expr_text);

    update_display(display_label);
}

void create_calculator(lv_obj_t * parent)
{
    // Remove any default padding from parent
    lv_obj_set_style_pad_all(parent, 0, 0);

    // Create expression label
    g_expr_label = lv_label_create(parent);
    lv_label_set_text(g_expr_label, "");
    lv_obj_set_width(g_expr_label, lv_pct(90));
    lv_obj_set_height(g_expr_label, 20);
    lv_obj_align(g_expr_label, LV_ALIGN_TOP_MID, 0, 5); // 5px from top
    lv_obj_set_style_text_align(g_expr_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_font(g_expr_label, &lv_font_montserrat_14, 0);

    // Create display - wider and with better styling
    lv_obj_t * display = lv_label_create(parent);
    lv_label_set_text(display, "0");
    lv_obj_set_width(display, lv_pct(90)); // 90% of parent width
    lv_obj_set_height(display, 50);
    lv_obj_align(display, LV_ALIGN_TOP_MID, 0, 28); // 20px from top
    lv_obj_set_style_text_align(display, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_bg_color(display, lv_color_hex(0xEEEEEE), 0);
    lv_obj_set_style_bg_opa(display, LV_OPA_100, 0);
    lv_obj_set_style_radius(display, 5, 0);
    lv_obj_set_style_pad_all(display, 10, 0);
    lv_obj_set_style_text_font(display, &lv_font_montserrat_18, 0);
    lv_obj_set_style_border_width(display, 1, 0);
    lv_obj_set_style_border_color(display, lv_color_hex(0xCCCCCC), 0);

    // Create button grid - same width as display
    lv_obj_t * grid = lv_obj_create(parent);
    lv_obj_set_width(grid, lv_pct(90)); // Same width as display
    lv_obj_set_height(grid, 250);
    lv_obj_align_to(grid, display, LV_ALIGN_OUT_BOTTOM_MID, 0, 20); // 20px below display
    lv_obj_set_layout(grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_all(grid, 20, 0);
    lv_obj_set_style_bg_opa(grid, LV_OPA_0, 0); // Transparent background

    // Grid layout: 4 equal columns, 5 rows
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
                                   LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);
    lv_obj_set_style_pad_row(grid, 8, 0);    // 8px between rows
    lv_obj_set_style_pad_column(grid, 8, 0); // 8px between columns

    // Button labels
    const char * btn_labels[] = {"C", " ", " ", " ", "7", "8", "9", "/", "4", "5",
                                 "6", "*", "1", "2", "3", "-", "0", ".", "=", "+"};

    // Create buttons with consistent styling
    for(int i = 0; i < 20; i++) {
        if(strcmp(btn_labels[i], " ") == 0) continue;

        lv_obj_t * btn = lv_btn_create(grid);
        lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i % 4, 1, LV_GRID_ALIGN_STRETCH, i / 4, 1);

        // Button styling
        lv_obj_set_style_radius(btn, 8, 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_100, 0);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0xDDDDDD), 0);
        lv_obj_set_style_text_color(btn, lv_color_black(), 0);
        lv_obj_set_style_shadow_width(btn, 0, 0);

        // Special styling for operation buttons
        if(strchr("+-*/=", btn_labels[i][0])) {
            lv_obj_set_style_bg_color(btn, lv_color_hex(0x4285F4), 0);
            lv_obj_set_style_text_color(btn, lv_color_white(), 0);
        } else if(strcmp(btn_labels[i], "C") == 0) {
            lv_obj_set_style_bg_color(btn, lv_color_hex(0xEA4335), 0);
            lv_obj_set_style_text_color(btn, lv_color_white(), 0);
        }

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, btn_labels[i]);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_18, 0);
        lv_obj_center(label);

        lv_obj_add_event_cb(btn, button_cb, LV_EVENT_CLICKED, display);
    }

    // Make the equals button span 2 columns
    lv_obj_t * equals_btn = lv_obj_get_child(grid, 18); // "=" button is at index 18
    if(equals_btn) {
        lv_obj_set_grid_cell(equals_btn, LV_GRID_ALIGN_STRETCH, 2, 2, // Start at column 2, span 2 columns
                             LV_GRID_ALIGN_STRETCH, 4, 1);            // Row 4
    }
}

void calculator_open(lv_obj_t ** out_screen)
{
    AppWindow win = app_window_create("Calculator");

    // Add your UI to `win.content`
    create_calculator(win.content);

    *out_screen = win.screen;
}
