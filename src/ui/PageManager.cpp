#include "PageManager.h"
#include <Arduino.h>

// Include all page headers here
#include "pages/SpeedPage.h"
#include "pages/StatsPage.h"
#include "pages/InfoPage.h"

// Singleton instance
PageManager *PageManager::instance = nullptr;

PageManager &PageManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new PageManager();
    }
    return *instance;
}

void PageManager::init()
{
    // ============================================
    // ADD YOUR PAGES HERE
    // Pages will appear in the order they are added
    // ============================================

    addPage(new SpeedPage());
    addPage(new StatsPage());
    addPage(new InfoPage());

    // ============================================
    // Create all the pages
    createPages();
}

void PageManager::addPage(Page *page)
{
    pages.push_back(page);
    Serial.printf("PageManager: Registered page '%s' at index %d\n",
                  page->getName(), pages.size() - 1);
}

void PageManager::createPages()
{
    if (pages.empty())
    {
        Serial.println("PageManager: No pages registered!");
        return;
    }

    // Create tileview container
    tileview = lv_tileview_create(lv_scr_act());
    lv_obj_set_style_bg_color(tileview, lv_color_black(), 0);

    // Add event callback for tile changes
    lv_obj_add_event_cb(tileview, tileChangeCallback, LV_EVENT_VALUE_CHANGED, this);

    // Create tiles for each page
    for (size_t i = 0; i < pages.size(); i++)
    {
        // Determine swipe directions based on position
        lv_dir_t dirs = LV_DIR_NONE;

        if (i > 0)
        {
            dirs |= LV_DIR_LEFT; // Can swipe left to go to previous
        }
        if (i < pages.size() - 1)
        {
            dirs |= LV_DIR_RIGHT; // Can swipe right to go to next
        }

        // Create the tile
        lv_obj_t *tile = lv_tileview_add_tile(tileview, i, 0, dirs);

        // Assign tile to page and create its UI
        pages[i]->setTile(tile);
        pages[i]->create();

        Serial.printf("PageManager: Created page '%s' at position %d\n",
                      pages[i]->getName(), i);
    }

    // Notify first page that it's visible
    if (!pages.empty())
    {
        pages[0]->onEnter();
    }

    Serial.printf("PageManager: Initialized with %d pages\n", pages.size());
}

void PageManager::tileChangeCallback(lv_event_t *e)
{
    PageManager *manager = static_cast<PageManager *>(lv_event_get_user_data(e));
    lv_obj_t *tileview = lv_event_get_target(e);

    // Get the current tile
    lv_obj_t *currentTile = lv_tileview_get_tile_act(tileview);

    // Find which page index this corresponds to
    for (size_t i = 0; i < manager->pages.size(); i++)
    {
        if (manager->pages[i]->getTile() == currentTile)
        {
            if (manager->currentPageIndex != (int)i)
            {
                // Notify old page it's exiting
                manager->pages[manager->currentPageIndex]->onExit();

                // Update current index
                manager->currentPageIndex = i;

                // Notify new page it's entering
                manager->pages[i]->onEnter();

                Serial.printf("PageManager: Switched to page '%s' (index %d)\n",
                              manager->pages[i]->getName(), i);
            }
            break;
        }
    }
}

void PageManager::update()
{
    // Update only the current page for performance
    if (currentPageIndex >= 0 && currentPageIndex < (int)pages.size())
    {
        pages[currentPageIndex]->update();
    }
}

Page *PageManager::getPage(int index)
{
    if (index >= 0 && index < (int)pages.size())
    {
        return pages[index];
    }
    return nullptr;
}

void PageManager::goToPage(int index)
{
    if (index >= 0 && index < (int)pages.size() && tileview != nullptr)
    {
        lv_obj_set_tile(tileview, pages[index]->getTile(), LV_ANIM_ON);
    }
}
