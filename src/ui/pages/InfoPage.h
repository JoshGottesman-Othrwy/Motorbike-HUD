#pragma once
#include "../Page.h"
#include "../Theme.h"

/**
 * Info page showing system status.
 * Displays WiFi status, IP address, and module detection status.
 */
class InfoPage : public Page
{
private:
    // UI Elements
    lv_obj_t *wifiStatusLabel = nullptr;
    lv_obj_t *wifiSSIDLabel = nullptr;
    lv_obj_t *wifiIPLabel = nullptr;
    lv_obj_t *moduleGPSLabel = nullptr;
    lv_obj_t *moduleIMULabel = nullptr;
    lv_obj_t *moduleMagnetometerLabel = nullptr;

    // Debug UI Elements
    lv_obj_t *debugFrameCounter = nullptr;
    lv_obj_t *debugFPS = nullptr;
    lv_obj_t *debugUptime = nullptr;

    // Debug tracking variables
    uint32_t frameCount = 0;
    uint32_t lastFPSUpdate = 0;
    uint32_t framesThisSecond = 0;
    float currentFPS = 0.0f;

public:
    InfoPage() : Page("Info") {}

    void create() override;
    void update() override;
};
