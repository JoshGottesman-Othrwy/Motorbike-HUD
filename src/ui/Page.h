#pragma once
#include <lvgl.h>

/**
 * Base class for all display pages.
 * Each page should inherit from this and implement the required methods.
 *
 * To create a new page:
 * 1. Create a new .h/.cpp file in the pages/ folder
 * 2. Inherit from Page and implement create() and optionally update()
 * 3. Add the page to the pages array in PageManager.cpp
 */
class Page
{
protected:
    lv_obj_t *tile = nullptr; // The tile object for this page
    const char *name;         // Page name for debugging

public:
    Page(const char *pageName) : name(pageName) {}
    virtual ~Page() = default;

    /**
     * Set the tile object for this page.
     * Called by PageManager when setting up the tileview.
     */
    void setTile(lv_obj_t *tileObj)
    {
        tile = tileObj;
        lv_obj_set_style_bg_color(tile, lv_color_black(), 0);
    }

    /**
     * Get the tile object for this page.
     */
    lv_obj_t *getTile() const { return tile; }

    /**
     * Get the page name.
     */
    const char *getName() const { return name; }

    /**
     * Create the UI elements for this page.
     * Called once during initialization after the tile is set.
     * Must be implemented by each page.
     */
    virtual void create() = 0;

    /**
     * Update the page with new data.
     * Called periodically from the main loop.
     * Override this if your page needs dynamic updates.
     */
    virtual void update() {}

    /**
     * Called when this page becomes visible (user swipes to it).
     * Override to perform actions when page is shown.
     */
    virtual void onEnter() {}

    /**
     * Called when this page is no longer visible (user swipes away).
     * Override to perform cleanup or pause updates.
     */
    virtual void onExit() {}
};
