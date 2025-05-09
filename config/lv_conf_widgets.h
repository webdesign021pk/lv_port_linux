/**
 * @file config/lv_conf_widgets.h
 * Configuration file for v9.3.0-dev
 */

#ifndef LV_CONF_WIDGETS_H
#define LV_CONF_WIDGETS_H

/*==================
 * WIDGETS
 *================*/
/* Documentation for widgets can be found here: https://docs.lvgl.io/master/details/widgets/index.html . */

/** 1: Causes these widgets to be given default values at creation time.
 *  - lv_buttonmatrix_t:  Get default maps:  {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""}, else map not set.
 *  - lv_checkbox_t    :  String label set to "Check box", else set to empty string.
 *  - lv_dropdown_t    :  Options set to "Option 1", "Option 2", "Option 3", else no values are set.
 *  - lv_roller_t      :  Options set to "Option 1", "Option 2", "Option 3", "Option 4", "Option 5", else no values are set.
 *  - lv_label_t       :  Text set to "Text", else empty string.
 * */
#define LV_WIDGETS_HAS_DEFAULT_VALUE  1

#define LV_USE_ANIMIMG    1

#define LV_USE_ARC        1

#define LV_USE_BAR        1

#define LV_USE_BUTTON        1

#define LV_USE_BUTTONMATRIX  1

#define LV_USE_CALENDAR   1
#if LV_USE_CALENDAR
    #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
    #if LV_CALENDAR_WEEK_STARTS_MONDAY
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
    #else
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
    #endif

    #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
    #define LV_USE_CALENDAR_HEADER_ARROW 1
    #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
    #define LV_USE_CALENDAR_CHINESE 0
#endif  /*LV_USE_CALENDAR*/

#define LV_USE_CANVAS     1

#define LV_USE_CHART      1

#define LV_USE_CHECKBOX   1

#define LV_USE_DROPDOWN   1   /**< Requires: lv_label */

#define LV_USE_IMAGE      1   /**< Requires: lv_label */

#define LV_USE_IMAGEBUTTON     1

#define LV_USE_KEYBOARD   1

#define LV_USE_LABEL      1
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1   /**< Enable selecting text of the label */
    #define LV_LABEL_LONG_TXT_HINT 1    /**< Store some extra info in labels to speed up drawing of very long text */
    #define LV_LABEL_WAIT_CHAR_COUNT 3  /**< The count of wait chart */
#endif

#define LV_USE_LED        1

#define LV_USE_LINE       1

#define LV_USE_LIST       1

#define LV_USE_LOTTIE     1

#define LV_USE_MENU       1

#define LV_USE_MSGBOX     1

#define LV_USE_ROLLER     1   /**< Requires: lv_label */

#define LV_USE_SCALE      1

#define LV_USE_SLIDER     1   /**< Requires: lv_bar */

#define LV_USE_SPAN       1
#if LV_USE_SPAN
    /** A line of text can contain this maximum number of span descriptors. */
    #define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

#define LV_USE_SPINBOX    1

#define LV_USE_SPINNER    1

#define LV_USE_SWITCH     1

#define LV_USE_TABLE      1

#define LV_USE_TABVIEW    1

#define LV_USE_TEXTAREA   1   /**< Requires: lv_label */
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /**< [ms] */
#endif

#define LV_USE_TILEVIEW   1

#define LV_USE_WIN        1

#define LV_USE_3DTEXTURE  0

#endif /*LV_CONF_WIDGETS_H*/