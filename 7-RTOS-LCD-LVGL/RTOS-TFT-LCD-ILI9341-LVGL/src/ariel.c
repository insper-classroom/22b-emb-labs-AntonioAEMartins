/*******************************************************************************
 * Size: 20 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/
#define LV_LVGL_H_INCLUDE_SIMPLE
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef ARIEL
#define ARIEL 1
#endif

#if ARIEL

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0043 "C" */
    0xf, 0x81, 0xc3, 0x18, 0xc, 0xc0, 0x6c, 0x0,
    0x60, 0x3, 0x0, 0x18, 0x0, 0xc0, 0x6, 0x0,
    0x30, 0x6, 0xc0, 0x66, 0x3, 0x1c, 0x30, 0x3e,
    0x0,

    /* U+0045 "E" */
    0xff, 0xf8, 0x3, 0x0, 0x60, 0xc, 0x1, 0x80,
    0x30, 0x7, 0xfe, 0xc0, 0x18, 0x3, 0x0, 0x60,
    0xc, 0x1, 0x80, 0x3f, 0xf8,

    /* U+0046 "F" */
    0xff, 0xf0, 0xc, 0x3, 0x0, 0xc0, 0x30, 0xc,
    0x3, 0xfe, 0xc0, 0x30, 0xc, 0x3, 0x0, 0xc0,
    0x30, 0xc, 0x0,

    /* U+004C "L" */
    0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3, 0x1,
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3,
    0xfe,

    /* U+004D "M" */
    0xe0, 0x1f, 0xc0, 0x7f, 0x3, 0xfc, 0xf, 0xd0,
    0x2f, 0x61, 0xbd, 0x86, 0xf2, 0x13, 0xcc, 0xcf,
    0x33, 0x3c, 0x48, 0xf1, 0xe3, 0xc7, 0x8f, 0xc,
    0x3c, 0x30, 0xc0,

    /* U+004E "N" */
    0xc0, 0x7c, 0xf, 0xc1, 0xf8, 0x3d, 0x87, 0xb0,
    0xf3, 0x1e, 0x73, 0xc6, 0x78, 0x6f, 0xd, 0xe0,
    0xfc, 0x1f, 0x81, 0xf0, 0x18,

    /* U+004F "O" */
    0xf, 0xc0, 0x61, 0x83, 0x3, 0x18, 0x6, 0xc0,
    0xb, 0x0, 0x3c, 0x0, 0xf0, 0x3, 0xc0, 0xf,
    0x0, 0x3c, 0x0, 0xd8, 0x6, 0x30, 0x30, 0x61,
    0x80, 0xfc, 0x0,

    /* U+0050 "P" */
    0xff, 0x98, 0x1b, 0x1, 0xe0, 0x3c, 0x7, 0x80,
    0xf0, 0x1e, 0x6, 0xff, 0x98, 0x3, 0x0, 0x60,
    0xc, 0x1, 0x80, 0x30, 0x0,

    /* U+0052 "R" */
    0xff, 0x8c, 0xc, 0xc0, 0x6c, 0x6, 0xc0, 0x6c,
    0x6, 0xc0, 0xcf, 0xf8, 0xc3, 0xc, 0x18, 0xc1,
    0xcc, 0xc, 0xc0, 0xec, 0x6, 0xc0, 0x70,

    /* U+0053 "S" */
    0xf, 0x83, 0xe, 0x60, 0x66, 0x6, 0x60, 0x7,
    0xc0, 0x3f, 0x81, 0xfe, 0x3, 0xf0, 0x7, 0xc0,
    0x36, 0x3, 0x60, 0x23, 0x6, 0x1f, 0x80,

    /* U+0054 "T" */
    0xff, 0xe0, 0xc0, 0x18, 0x3, 0x0, 0x60, 0xc,
    0x1, 0x80, 0x30, 0x6, 0x0, 0xc0, 0x18, 0x3,
    0x0, 0x60, 0xc, 0x1, 0x80,

    /* U+00B0 "°" */
    0x76, 0xe3, 0x1d, 0xb8
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 231, .box_w = 13, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 25, .adv_w = 213, .box_w = 11, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 46, .adv_w = 195, .box_w = 10, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 65, .adv_w = 178, .box_w = 9, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 267, .box_w = 14, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 231, .box_w = 11, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 130, .adv_w = 249, .box_w = 14, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 157, .adv_w = 213, .box_w = 11, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 178, .adv_w = 231, .box_w = 12, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 213, .box_w = 12, .box_h = 15, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 224, .adv_w = 195, .box_w = 11, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 128, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 10}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x2, 0x3, 0x9, 0xa, 0xb, 0xc, 0xd,
    0xf, 0x10, 0x11, 0x6d
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 67, .range_length = 110, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 12, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    4, 11,
    9, 11,
    11, 7
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -24, -6, -6
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 3,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t ariel = {
#else
lv_font_t ariel = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if ARIEL*/

