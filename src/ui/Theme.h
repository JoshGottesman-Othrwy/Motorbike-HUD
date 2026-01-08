#pragma once
#include <lvgl.h>

/**
 * Shared color theme for the display.
 * Include this header in any page that needs access to the color palette.
 */
namespace Theme
{
    // Initialize all theme colors - call once at startup
    inline void init()
    {
        // Colors are initialized on first use via the getter functions
    }

    // Color getters - these return consistent colors across all pages
    inline lv_color_t white() { return lv_color_white(); }
    inline lv_color_t black() { return lv_color_black(); }
    inline lv_color_t grey() { return lv_color_make(0xAA, 0xAA, 0xAA); }
    inline lv_color_t darkGrey() { return lv_color_make(0x5C, 0x5C, 0x5C); }
    inline lv_color_t veryDarkGrey() { return lv_color_make(0x32, 0x32, 0x32); }
    inline lv_color_t green() { return lv_color_make(0x4C, 0xAF, 0x50); }
    inline lv_color_t yellow() { return lv_color_make(0xFF, 0xEB, 0x3B); }
    inline lv_color_t orange() { return lv_color_make(0xFF, 0x98, 0x00); }
    inline lv_color_t red() { return lv_color_make(0xAA, 0x43, 0x36); }
}
