#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lvgl.h"

static lv_obj_t * mem_widget = NULL;
static lv_timer_t * mem_timer = NULL;

static void update_mem_usage(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);

    static char buf[128];
    snprintf(buf, sizeof(buf),
        "Used: %u\nFree: %u\nFrag: %d%%",
        (unsigned)(mon.total_size - mon.free_size),
        (unsigned)(mon.free_size),
        mon.frag_pct);

    lv_label_set_text(mem_widget, buf);
}

void init_memory_widget(void)
{
    mem_widget = lv_label_create(lv_layer_top());  // Always on top
    lv_obj_set_style_text_color(mem_widget, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(mem_widget, &lv_font_montserrat_10, 0);
    lv_obj_align(mem_widget, LV_ALIGN_TOP_RIGHT, -10, 10);

    mem_timer = lv_timer_create(update_mem_usage, 1000, NULL);  // update every 1s
}

#endif // DEBUG_H