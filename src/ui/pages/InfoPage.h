#pragma once
#include "../Page.h"
#include "../Theme.h"
#include "../../sensors/GPS.h"
#include "../../display.h"
#include "../../wifi/WiFiOTA.h"

// External sensor instances from main.cpp
extern GPS gps;
extern Display display;
extern WiFiOTA wifiOTA;

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
    lv_obj_t *wifiOTAStatusLabel = nullptr;
    lv_obj_t *wifiRefreshBtn = nullptr;
    lv_obj_t *moduleGPSLabel = nullptr;
    lv_obj_t *moduleIMULabel = nullptr;
    lv_obj_t *moduleMagnetometerLabel = nullptr;

    // Battery UI Elements
    lv_obj_t *batteryVoltageLabel = nullptr;
    lv_obj_t *batteryStatusLabel = nullptr;
    lv_obj_t *batteryPercentLabel = nullptr;
    lv_obj_t *batteryCurrentLabel = nullptr;

    // Debug UI Elements
    lv_obj_t *debugFrameCounter = nullptr;
    lv_obj_t *debugFPS = nullptr;
    lv_obj_t *debugUptime = nullptr;

    // Debug tracking variables
    uint32_t frameCount = 0;
    uint32_t lastFPSUpdate = 0;
    uint32_t framesThisSecond = 0;
    float currentFPS = 0.0f;

    static void onRefreshBtnClicked(lv_event_t *e);

public:
    InfoPage() : Page("Info") {}

    void create() override;
    void update() override;
};
