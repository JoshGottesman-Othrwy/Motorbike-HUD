#pragma once
#include <lvgl.h>
#include <vector>
#include "Page.h"

/**
 * PageManager handles the tileview and all registered pages.
 *
 * To add a new page:
 * 1. Create your page class inheriting from Page
 * 2. Add it to the pages vector in PageManager::init()
 *
 * The pages will be arranged horizontally in the order they are added.
 */
class PageManager
{
private:
    lv_obj_t *tileview = nullptr;
    std::vector<Page *> pages;
    int currentPageIndex = 0;

    // Singleton instance
    static PageManager *instance;

    // Private constructor for singleton
    PageManager() = default;

    // Callback for tile change events
    static void tileChangeCallback(lv_event_t *e);

public:
    // Get singleton instance
    static PageManager &getInstance();

    // Delete copy constructor and assignment
    PageManager(const PageManager &) = delete;
    PageManager &operator=(const PageManager &) = delete;

    /**
     * Initialize the page manager with all pages.
     * Call this after LVGL is initialized.
     */
    void init();

    /**
     * Register a page with the manager.
     * Pages are added in order from left to right.
     */
    void addPage(Page *page);

    /**
     * Create the tileview and all registered pages.
     * Call this after all pages have been added.
     */
    void createPages();

    /**
     * Update all pages (or just the current page for performance).
     * Call this from the main loop.
     */
    void update();

    /**
     * Get a page by index.
     */
    Page *getPage(int index);

    /**
     * Get the number of registered pages.
     */
    int getPageCount() const { return pages.size(); }

    /**
     * Get the current page index.
     */
    int getCurrentPageIndex() const { return currentPageIndex; }

    /**
     * Navigate to a specific page by index.
     */
    void goToPage(int index);
};
