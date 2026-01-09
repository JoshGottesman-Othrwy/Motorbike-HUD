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
    uint8_t cachedHour = 255;        // Invalid initial value to force first update
    uint8_t cachedMinute = 255;      // Invalid initial value to force first update
    bool cachedTimeWasValid = false; // Track when GPS time becomes valid
    int32_t cachedSpeed = -1;        // Cached speed for change detection
    bool firstUpdate = true;         // Force update on first run to sync display with actual state
    bool isPageActive = false;       // Track if this page is currently visible

    // Speed animation state
    int32_t targetSpeed = 0;                                     // Target speed we're animating towards
    int32_t displayedSpeed = 0;                                  // Currently displayed speed
    uint32_t lastSpeedUpdate = 0;                                // Last time speed display was incremented
    static constexpr uint32_t SPEED_INCREMENT_INTERVAL_MS = 100; // Tunable: animation speed

    // Helper methods for clean, readable code
    void updateSatelliteDisplay();
    void updateGPSStatusDisplay();
    void updateClockDisplay();
    void updateSpeedDisplay();
    lv_color_t getStatusColor(GPSStatus status);
    const char *getStatusText(GPSStatus status);

public:
    SpeedPage() : Page("Speed") {}

    void create() override;
    void update() override;
    void onEnter() override { isPageActive = true; }
    void onExit() override { isPageActive = false; }
};
