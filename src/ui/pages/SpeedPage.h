#pragma once
#include "../Page.h"
#include "../Theme.h"
#include "../../sensors/GPS.h"

// External font declarations
LV_FONT_DECLARE(RobotoBlack_60);
LV_FONT_DECLARE(RobotoBlack_200);

// Forward declaration - GPS instance is defined in main.cpp
extern GPS gps;

/**
 * Main speed display page.
 * Shows current speed, satellite count, GPS quality, clock, and recent max speed.
 */
class SpeedPage : public Page
{
private:
    // UI Elements
    lv_obj_t *satsLabel = nullptr;
    lv_obj_t *gpsQualityLabel = nullptr;
    lv_obj_t *recentMaxLabel = nullptr;
    lv_obj_t *recentMaxUnits = nullptr;
    lv_obj_t *mainSpeedUnits = nullptr;
    lv_obj_t *mainSpeed = nullptr;
    lv_obj_t *clockDisplay = nullptr;
    lv_obj_t *recentMaxValue = nullptr;

    // Cached state for change detection (only update display when values change)
    int32_t cachedSatelliteCount = -1;
    GPSStatus cachedGPSStatus = GPSStatus::NotConnected;
    bool firstUpdate = true; // Force update on first run to sync display with actual state

    // Helper methods for clean, readable code
    void updateSatelliteDisplay();
    void updateGPSStatusDisplay();
    lv_color_t getStatusColor(GPSStatus status);
    const char *getStatusText(GPSStatus status);

public:
    SpeedPage() : Page("Speed") {}

    void create() override;
    void update() override;
};
