#pragma once
#include <Arduino.h>
#include <LilyGo_AMOLED.h>
#include "ui/lvgl/LV_Helper.h"
#include "ui/PageManager.h"

/**
 * Display class - handles hardware initialization and delegates UI to PageManager.
 * This is now a thin wrapper around the hardware and PageManager.
 */
class Display
{
private:
    LilyGo_AMOLED amoled;

public:
    Display();

    /**
     * Initialize the display hardware and all pages.
     * Returns true on success.
     */
    bool begin();

    /**
     * Update the display - call this from the main loop.
     * Delegates to PageManager to update the current page.
     */
    void update();

    /**
     * Set display brightness (0-255).
     */
    void setBrightness(uint8_t brightness);

    /**
     * Get reference to the AMOLED object (if needed for advanced features).
     */
    LilyGo_AMOLED &getAmoled() { return amoled; }
};
