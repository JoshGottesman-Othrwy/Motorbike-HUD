#pragma once
#include "../Page.h"
#include "../Theme.h"
#include "../../sensors/GPS.h"

// External font declarations
LV_FONT_DECLARE(RobotoBlack_60);

// External GPS instance from main.cpp
extern GPS gps;

/**
 * Driving statistics page.
 * Shows satellite count, current speed, and 0-60 time.
 */
class StatsPage : public Page
{
private:
    // UI Elements
    lv_obj_t *satsLabel = nullptr;
    lv_obj_t *speedLabel = nullptr;
    lv_obj_t *speedUnits = nullptr;
    lv_obj_t *zeroToSixtyLabel = nullptr;
    lv_obj_t *zeroToSixtyUnits = nullptr;

    // Page state and caching
    bool isPageActive = false;
    float cachedSpeed = -1.0f;     // Cached speed for change detection
    int32_t cachedSatellites = -1; // Cached satellite count

    // Helper methods
    void updateSpeedDisplay();
    void updateSatelliteDisplay();

public:
    StatsPage() : Page("Stats") {}

    void create() override;
    void update() override;
    void onEnter() override { isPageActive = true; }
    void onExit() override { isPageActive = false; }
};
