/**
 * @file config/lv_conf_fonts.h
 * Configuration file for v9.3.0-dev
 */

#ifndef LV_CONF_FONTS_H
#define LV_CONF_FONTS_H
/*==================
 *   FONT USAGE
 *===================*/

/* Montserrat fonts with ASCII range and some symbols using bpp = 4
 * https://fonts.google.com/specimen/Montserrat */
#define LV_FONT_MONTSERRAT_8 0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 1
#define LV_FONT_MONTSERRAT_36 1
#define LV_FONT_MONTSERRAT_38 1
#define LV_FONT_MONTSERRAT_40 1
#define LV_FONT_MONTSERRAT_42 1
#define LV_FONT_MONTSERRAT_44 1
#define LV_FONT_MONTSERRAT_46 1
#define LV_FONT_MONTSERRAT_48 1

/* Demonstrate special features */
#define LV_FONT_MONTSERRAT_28_COMPRESSED 1
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 1
#define LV_FONT_SIMSUN_14_CJK 0 /**< 1000 most common CJK radicals */
#define LV_FONT_SIMSUN_16_CJK 1
#define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK 0 /**< 1338 most common CJK radicals */
#define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK 0 /**< 1338 most common CJK radicals */

/** Pixel perfect monospaced fonts */
#define LV_FONT_UNSCII_8 1
#define LV_FONT_UNSCII_16 0

/** Optionally declare custom fonts here.
 *
 *  You can use any of these fonts as the default font too and they will be available
 *  globally.  Example:
 *
 *  @code
 *  #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)
 *  @endcode
 */
#define LV_FONT_CUSTOM_DECLARE

/** Always set a default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/** Enable handling large font and/or fonts with a lot of characters.
 *  The limit depends on the font size, font face and bpp.
 *  A compiler error will be triggered if a font needs it. */
#define LV_FONT_FMT_TXT_LARGE 1

/** Enables/disables support for compressed fonts. */
#define LV_USE_FONT_COMPRESSED 0

/** Enable drawing placeholders when glyph dsc is not found. */
#define LV_USE_FONT_PLACEHOLDER 1

#endif /* LV_CONF_FONTS_H */