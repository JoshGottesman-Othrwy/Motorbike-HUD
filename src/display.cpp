#include "display.h"

Display::Display()
{
    // Constructor
}

bool Display::begin()
{
    Serial.print("Initializing display hardware... ");

    // Initialize the AMOLED display
    bool rslt = amoled.begin();
    if (!rslt)
    {
        Serial.println("FAILED");
        Serial.println("Display error: Cannot detect board model");
        return false;
    }
    Serial.println("OK");

    // Initialize LVGL helper (non-DMA - this AMOLED uses SPIClass, not ESP-IDF SPI)
    beginLvglHelper(amoled);

    // Set max brightness
    setBrightness(255);

    // Set black background
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    // Initialize the page manager (creates all pages)
    Serial.println("Initializing page manager...");
    PageManager::getInstance().init();

    return true;
}

void Display::update()
{
    // Update the current page through PageManager
    PageManager::getInstance().update();
}

void Display::setBrightness(uint8_t brightness)
{
    amoled.setBrightness(brightness);
}
