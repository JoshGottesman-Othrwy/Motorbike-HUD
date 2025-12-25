#pragma once
#include <Arduino.h>
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>

class Display
{
private:
    // Display objects
    LilyGo_AMOLED amoled;

    // Tileview and pages
    lv_obj_t *tileview;
    lv_obj_t *speedPage;
    lv_obj_t *midPage;
    lv_obj_t *infoPage;

    // UI Elements
    lv_obj_t *TopLabel;
    lv_obj_t *BottomLabel;
    lv_obj_t *TopUnits;
    lv_obj_t *BottomUnits;
    lv_obj_t *MainVariable;
    lv_obj_t *AuxVariable;
    lv_obj_t *MainUnits;
    lv_obj_t *TopVariable;
    lv_obj_t *BottomVariable;
    lv_obj_t *debugDisp;

    // Info page elements
    lv_obj_t *wifiStatusLabel;
    lv_obj_t *wifiSSIDLabel;
    lv_obj_t *wifiIPLabel;
    lv_obj_t *moduleGPSLabel;
    lv_obj_t *moduleIMULabel;
    lv_obj_t *midSatsLabel;
    lv_obj_t *midGpsStatusLabel;
    lv_obj_t *midSpeedLabel;
    lv_obj_t *midSpeedUnits;

    // Display state
    bool isFaded = false;
    int lastHdopState = -1;
    bool debug = false;
    int speedInt = 0;

    // Update frequencies
    uint32_t updateDisplayInterval = 33;
    uint32_t prevDispUpdate = 0;
    uint32_t updateSpeed = 100;
    uint32_t prevSpeedUpdate = 0;

    // Colors
    lv_color_t grey;
    lv_color_t darkGrey;
    lv_color_t veryDarkGrey;
    lv_color_t green;
    lv_color_t yellow;
    lv_color_t orange;
    lv_color_t red;

    // Private methods
    void initializeColors();
    void createTileview();
    void createUIElements();
    void createMiddlePage();
    void createInfoPage();
    void startUpScreen();
    void updateSpeedAnimation(float targetSpeed);

public:
    Display();
    bool begin();
    void loop();

    // Public methods to update display data
    void updateGPSData(float speed, float speedMax, float hdop, int sats, float zeroToSixtyTime);
    void updateWiFiInfo(bool connected, const char *ssid, const char *ip, const char *status);
    void updateModuleStatus(bool gpsDetected, bool imuDetected);
    void setFirstFix(bool hasFix);
    void setBrightness(uint8_t brightness);

    // Getter for amoled object (needed for button integration)
    LilyGo_AMOLED &getAmoled() { return amoled; }
};
