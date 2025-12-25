#include "display.h"

// External font declarations
LV_FONT_DECLARE(RobotoBlack_60);
LV_FONT_DECLARE(RobotoBlack_200);

Display::Display()
{
    // Constructor
}

bool Display::begin()
{
    Serial.print("Initializing display... ");

    // Automatically determine the access device
    bool rslt = amoled.begin();
    if (!rslt)
    {
        Serial.println("FAILED");
        Serial.println("Display error: Cannot detect board model");
        return false;
    }

    Serial.println("OK");

    // Register lvgl helper
    beginLvglHelper(amoled);
    setBrightness(255); // Max Brightness

    // Initialize colors
    initializeColors();

    // Black screen
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    // Create tileview with pages
    createTileview();

    // Create UI elements
    createUIElements();

    // Create driving stats page
    createDrivingStatsPage();

    // Create info page
    createInfoPage();

    // Run startup screen
    startUpScreen();

    return true;
}

void Display::initializeColors()
{
    grey = lv_color_make(0xAA, 0xAA, 0xAA);
    darkGrey = lv_color_make(0x5C, 0x5C, 0x5C);
    veryDarkGrey = lv_color_make(0x32, 0x32, 0x32);
    green = lv_color_make(0x4C, 0xAF, 0x50);
    yellow = lv_color_make(0xFF, 0xEB, 0x3B);
    orange = lv_color_make(0xFF, 0x98, 0x00);
    red = lv_color_make(0xAA, 0x43, 0x36);
}

void Display::createTileview()
{
    // Create tileview container
    tileview = lv_tileview_create(lv_scr_act());
    lv_obj_set_style_bg_color(tileview, lv_color_black(), 0);

    // Create main speed page (tile 0, 0) - can swipe right to driving stats page
    speedPage = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);
    lv_obj_set_style_bg_color(speedPage, lv_color_black(), 0);

    // Create driving stats page (tile 1, 0) - can swipe left to speed or right to info
    driveStats = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    lv_obj_set_style_bg_color(driveStats, lv_color_black(), 0);

    // Create info page (tile 2, 0) - can swipe left back to driving stats page
    infoPage = lv_tileview_add_tile(tileview, 2, 0, LV_DIR_LEFT);
    lv_obj_set_style_bg_color(infoPage, lv_color_black(), 0);
}

void Display::createUIElements()
{
    // Set label formats/positions - create on speed page instead of screen
    TopLabel = lv_label_create(speedPage);
    lv_obj_set_style_text_font(TopLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(TopLabel, lv_color_white(), 0);
    lv_obj_align(TopLabel, LV_ALIGN_TOP_LEFT, 5, 5);

    BottomLabel = lv_label_create(speedPage);
    lv_obj_set_style_text_font(BottomLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(BottomLabel, lv_color_white(), 0);
    lv_obj_align(BottomLabel, LV_ALIGN_TOP_LEFT, 5, 125);

    AuxVariable = lv_label_create(speedPage);
    lv_obj_set_style_text_font(AuxVariable, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(AuxVariable, grey, 0);
    lv_obj_align(AuxVariable, LV_ALIGN_TOP_LEFT, 105, 5);

    BottomUnits = lv_label_create(speedPage);
    lv_obj_set_style_text_font(BottomUnits, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(BottomUnits, lv_color_white(), 0);
    lv_obj_align(BottomUnits, LV_ALIGN_TOP_LEFT, 168, 185);

    MainUnits = lv_label_create(speedPage);
    lv_obj_set_style_text_font(MainUnits, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(MainUnits, grey, 0);
    lv_obj_align(MainUnits, LV_ALIGN_BOTTOM_RIGHT, 0, -10);

    MainVariable = lv_label_create(speedPage);
    lv_obj_set_style_text_font(MainVariable, &RobotoBlack_200, 0);
    lv_obj_set_style_text_color(MainVariable, lv_color_white(), 0);
    lv_obj_align(MainVariable, LV_ALIGN_BOTTOM_RIGHT, 0, -25);

    clockDisplay = lv_label_create(speedPage);
    lv_obj_set_style_text_font(clockDisplay, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(clockDisplay, lv_color_white(), 0);
    lv_obj_align(clockDisplay, LV_ALIGN_BOTTOM_RIGHT, -350, -130);

    BottomVariable = lv_label_create(speedPage);
    lv_obj_set_style_text_font(BottomVariable, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(BottomVariable, lv_color_white(), 0);
    lv_obj_align(BottomVariable, LV_ALIGN_BOTTOM_RIGHT, -370, -10);

    debugDisp = lv_label_create(speedPage);
    lv_obj_set_style_text_font(debugDisp, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(debugDisp, green, 0);
    lv_obj_align(debugDisp, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_clear_flag(debugDisp, LV_OBJ_FLAG_HIDDEN);

    // Create Static Labels
    lv_label_set_text_fmt(TopLabel, "Sats.");
    lv_label_set_text_fmt(BottomLabel, "Recent Max.");
    lv_label_set_text_fmt(AuxVariable, "(Low)");
    lv_label_set_text_fmt(BottomUnits, "mph");
    lv_label_set_text_fmt(MainUnits, "mph");
}

void Display::createDrivingStatsPage()
{
    // Sat status in top-left (mirrors main screen label)
    midSatsLabel = lv_label_create(driveStats);
    lv_obj_set_style_text_font(midSatsLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(midSatsLabel, lv_color_white(), 0);
    lv_obj_align(midSatsLabel, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_label_set_text(midSatsLabel, "Sats. --");

    // Speed display at top center (same style as recent max on first screen)
    midSpeedLabel = lv_label_create(driveStats);
    lv_obj_set_style_text_font(midSpeedLabel, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(midSpeedLabel, lv_color_white(), 0);
    lv_obj_align(midSpeedLabel, LV_ALIGN_TOP_LEFT, 200, 5);
    lv_label_set_text(midSpeedLabel, "0.0");

    // mph units label
    midSpeedUnits = lv_label_create(driveStats);
    lv_obj_set_style_text_font(midSpeedUnits, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(midSpeedUnits, grey, 0);
    lv_obj_align(midSpeedUnits, LV_ALIGN_TOP_LEFT, 300, 15);
    lv_label_set_text(midSpeedUnits, "mph");

    // Horizontal line under speed display
    lv_obj_t *headerLine = lv_line_create(driveStats);
    static lv_point_t headerLinePoints[] = {{0, 0}, {480, 0}};
    lv_line_set_points(headerLine, headerLinePoints, 2);
    lv_obj_set_style_line_color(headerLine, grey, 0);
    lv_obj_set_style_line_width(headerLine, 4, 0);
    lv_obj_align(headerLine, LV_ALIGN_TOP_MID, 0, 70);

    // Vertical divider line splitting page in half
    lv_obj_t *dividerLine = lv_line_create(driveStats);
    static lv_point_t dividerLinePoints[] = {{0, 0}, {0, 120}};
    lv_line_set_points(dividerLine, dividerLinePoints, 2);
    lv_obj_set_style_line_color(dividerLine, grey, 0);
    lv_obj_set_style_line_width(dividerLine, 2, 0);
    lv_obj_align(dividerLine, LV_ALIGN_TOP_MID, 0, 100);

    // 0-60 time display (left side below divider)
    driveStatsZeroToSixty = lv_label_create(driveStats);
    lv_obj_set_style_text_font(driveStatsZeroToSixty, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(driveStatsZeroToSixty, lv_color_white(), 0);
    lv_obj_align(driveStatsZeroToSixty, LV_ALIGN_TOP_LEFT, 20, 87);
    lv_label_set_text(driveStatsZeroToSixty, "0.00");

    // 0-60 units label
    driveStatsZeroToSixtyUnits = lv_label_create(driveStats);
    lv_obj_set_style_text_font(driveStatsZeroToSixtyUnits, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(driveStatsZeroToSixtyUnits, grey, 0);
    lv_obj_align(driveStatsZeroToSixtyUnits, LV_ALIGN_TOP_LEFT, 150, 110);
    lv_label_set_text(driveStatsZeroToSixtyUnits, "s (0-60)");
}

void Display::createInfoPage()
{
    // Make info page scrollable
    lv_obj_set_scrollbar_mode(infoPage, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(infoPage, LV_DIR_VER);
    lv_obj_set_style_pad_all(infoPage, 0, 0);

    // Create a centered label for the info page title
    lv_obj_t *infoTitle = lv_label_create(infoPage);
    lv_obj_set_style_text_font(infoTitle, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(infoTitle, lv_color_white(), 0);
    lv_label_set_text(infoTitle, "INFO");
    lv_obj_align(infoTitle, LV_ALIGN_TOP_MID, 0, 20);

    // WiFi Status section
    lv_obj_t *wifiHeader = lv_label_create(infoPage);
    lv_obj_set_style_text_font(wifiHeader, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(wifiHeader, grey, 0);
    lv_label_set_text(wifiHeader, "WiFi Status: ");
    lv_obj_align(wifiHeader, LV_ALIGN_TOP_LEFT, 10, 70);

    wifiStatusLabel = lv_label_create(infoPage);
    lv_obj_set_style_text_font(wifiStatusLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(wifiStatusLabel, red, 0);
    lv_label_set_text(wifiStatusLabel, "Disconnected");
    lv_obj_align(wifiStatusLabel, LV_ALIGN_TOP_LEFT, 190, 70);

    wifiSSIDLabel = lv_label_create(infoPage);
    lv_obj_set_style_text_font(wifiSSIDLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(wifiSSIDLabel, grey, 0);
    lv_obj_set_width(wifiSSIDLabel, 430);
    lv_label_set_long_mode(wifiSSIDLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(wifiSSIDLabel, "SSID: ---");
    lv_obj_align(wifiSSIDLabel, LV_ALIGN_TOP_LEFT, 10, 110);

    wifiIPLabel = lv_label_create(infoPage);
    lv_obj_set_style_text_font(wifiIPLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(wifiIPLabel, grey, 0);
    lv_obj_set_width(wifiIPLabel, 430);
    lv_label_set_long_mode(wifiIPLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(wifiIPLabel, "IP: 0.0.0.0");
    lv_obj_align(wifiIPLabel, LV_ALIGN_TOP_LEFT, 10, 140);

    // Module Status section
    lv_obj_t *moduleHeader = lv_label_create(infoPage);
    lv_obj_set_style_text_font(moduleHeader, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(moduleHeader, grey, 0);
    lv_label_set_text(moduleHeader, "Module Status:");
    lv_obj_align(moduleHeader, LV_ALIGN_TOP_LEFT, 10, 200);

    moduleGPSLabel = lv_label_create(infoPage);
    lv_obj_set_style_text_font(moduleGPSLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(moduleGPSLabel, grey, 0);
    lv_label_set_text(moduleGPSLabel, "GPS: Not detected");
    lv_obj_align(moduleGPSLabel, LV_ALIGN_TOP_LEFT, 10, 240);

    moduleMagnetometerLabel = lv_label_create(infoPage);
    lv_obj_set_style_text_font(moduleMagnetometerLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(moduleMagnetometerLabel, grey, 0);
    lv_label_set_text(moduleMagnetometerLabel, "Magnetometer: Not detected");
    lv_obj_align(moduleMagnetometerLabel, LV_ALIGN_TOP_LEFT, 10, 270);

    moduleIMULabel = lv_label_create(infoPage);
    lv_obj_set_style_text_font(moduleIMULabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(moduleIMULabel, grey, 0);
    lv_label_set_text(moduleIMULabel, "IMU: Not detected");
    lv_obj_align(moduleIMULabel, LV_ALIGN_TOP_LEFT, 10, 300);
}

void Display::loop()
{
    // lv_task_handler() now called at start of main loop for better responsiveness
}

void Display::updateGPSData(float speed, float speedMax, float hdop, int sats, float zeroToSixtyTime)
{
    // Update Speed Animation
    if (millis() - prevSpeedUpdate > updateSpeed)
    {
        updateSpeedAnimation(speed);
        prevSpeedUpdate = millis();
    }

    // Update Display
    if (millis() - prevDispUpdate > updateDisplayInterval)
    {
        bool shouldFade = speedInt >= 100;
        if (shouldFade != isFaded)
        {
            lv_color_t color = shouldFade ? darkGrey : lv_color_white();
            lv_color_t labelColor = shouldFade ? darkGrey : grey;

            lv_obj_set_style_text_color(TopLabel, labelColor, 0);
            lv_obj_set_style_text_color(BottomUnits, color, 0);
            lv_obj_set_style_text_color(BottomLabel, labelColor, 0);
            lv_obj_set_style_text_color(clockDisplay, color, 0);
            lv_obj_set_style_text_color(BottomVariable, color, 0);
            lv_obj_set_style_text_color(AuxVariable, color, 0);

            isFaded = shouldFade;
        }

        lv_label_set_text_fmt(BottomVariable, "%.1f", speedMax);
        lv_label_set_text_fmt(TopLabel, "Sats. %u", sats);
        if (midSatsLabel)
        {
            lv_label_set_text_fmt(midSatsLabel, "Sats. %u", sats);
        }
        if (midSpeedLabel)
        {
            lv_label_set_text_fmt(midSpeedLabel, "%.1f", speed);
        }

        int hdopState;
        lv_color_t hdopStateColor;
        const char *hdopStateLabel;

        if (hdop < 1)
        {
            hdopState = 0;
            hdopStateColor = green;
            hdopStateLabel = " (Excellent)";
        }
        else if (hdop < 2)
        {
            hdopState = 1;
            hdopStateColor = green;
            hdopStateLabel = " (Good)";
        }
        else if (hdop < 5)
        {
            hdopState = 2;
            hdopStateColor = yellow;
            hdopStateLabel = " (Moderate)";
        }
        else if (hdop < 10)
        {
            hdopState = 3;
            hdopStateColor = orange;
            hdopStateLabel = " (Fair)";
        }
        else
        {
            hdopState = 4;
            hdopStateColor = red;
            hdopStateLabel = "(No Fix)";
        }

        if (hdopState != lastHdopState)
        {
            lv_obj_set_style_text_color(AuxVariable, hdopStateColor, 0);
            lv_label_set_text_fmt(AuxVariable, "%s", hdopStateLabel);
            lastHdopState = hdopState;
        }

        if (driveStatsZeroToSixty)
        {
            lv_label_set_text_fmt(driveStatsZeroToSixty, "%.2f", zeroToSixtyTime);
        }

        if (debug == 1)
        {
            lv_label_set_text_fmt(debugDisp, ".");
        }
        else
        {
            lv_label_set_text_fmt(debugDisp, " ");
        }
        debug = !debug;

        prevDispUpdate = millis();
    }
}

void Display::updateSpeedAnimation(float targetSpeed)
{
    if (speedInt < round(targetSpeed))
    {
        speedInt++;
        lv_label_set_text_fmt(MainVariable, "%u", speedInt);
    }
    else if (speedInt > round(targetSpeed))
    {
        speedInt--;
        lv_label_set_text_fmt(MainVariable, "%u", speedInt);
    }
}

void Display::updateTimeDisplay(bool timeValid, int hour, int minute)
{
    if (timeValid)
    {
        lv_label_set_text_fmt(clockDisplay, "%02d:%02d", hour, minute);
    }
    else
    {
        lv_label_set_text(clockDisplay, "--:--");
    }
}

void Display::updateWiFiInfo(bool connected, const char *ssid, const char *ip, const char *status)
{
    if (connected)
    {
        lv_obj_set_style_text_color(wifiStatusLabel, green, 0);
        lv_label_set_text(wifiStatusLabel, status);
        lv_label_set_text_fmt(wifiSSIDLabel, "SSID: %s", ssid);
        lv_label_set_text_fmt(wifiIPLabel, "IP: %s", ip);
    }
    else
    {
        lv_obj_set_style_text_color(wifiStatusLabel, yellow, 0);
        lv_label_set_text(wifiStatusLabel, status);
        lv_label_set_text(wifiSSIDLabel, "SSID: ---");
        lv_label_set_text(wifiIPLabel, "IP: 0.0.0.0");
    }
}

void Display::updateModuleStatus(bool gpsDetected, bool imuDetected, bool magnetometerDetected)
{
    if (gpsDetected)
    {
        lv_obj_set_style_text_color(moduleGPSLabel, green, 0);
        lv_label_set_text(moduleGPSLabel, "GPS: Detected");
    }
    else
    {
        lv_obj_set_style_text_color(moduleGPSLabel, red, 0);
        lv_label_set_text(moduleGPSLabel, "GPS: Not detected");
    }

    if (imuDetected)
    {
        lv_obj_set_style_text_color(moduleIMULabel, green, 0);
        lv_label_set_text(moduleIMULabel, "IMU: Detected");
    }
    else
    {
        lv_obj_set_style_text_color(moduleIMULabel, red, 0);
        lv_label_set_text(moduleIMULabel, "IMU: Not detected");
    }

    if (magnetometerDetected)
    {
        lv_obj_set_style_text_color(moduleMagnetometerLabel, green, 0);
        lv_label_set_text(moduleMagnetometerLabel, "Magnetometer: Detected");
    }
    else
    {
        lv_obj_set_style_text_color(moduleMagnetometerLabel, red, 0);
        lv_label_set_text(moduleMagnetometerLabel, "Magnetometer: Not detected");
    }
}

void Display::setFirstFix(bool hasFix)
{
    updateDisplayInterval = hasFix ? 33 : 2000; // update display frequency on fix
}

void Display::setBrightness(uint8_t brightness)
{
    amoled.setBrightness(brightness);
}

void Display::startUpScreen()
{
    Serial.println("Startup Screen");

    lv_obj_set_style_text_color(TopLabel, darkGrey, 0);
    lv_obj_set_style_text_color(BottomUnits, darkGrey, 0);
    lv_obj_set_style_text_color(BottomLabel, darkGrey, 0);
    lv_obj_set_style_text_color(clockDisplay, darkGrey, 0);
    lv_obj_set_style_text_color(BottomVariable, darkGrey, 0);
    lv_obj_set_style_text_color(AuxVariable, darkGrey, 0);

    lv_label_set_text_fmt(BottomVariable, "0.00");
    lv_label_set_text(clockDisplay, "00:00");

    for (int i = 0; i <= 100; i += 5)
    {
        lv_label_set_text_fmt(MainVariable, "%u", i);
        lv_task_handler();
        delay(10);
    }

    for (int i = 100; i >= 0; i -= 5)
    {
        lv_label_set_text_fmt(MainVariable, "%u", i);
        lv_task_handler();
        delay(10);
    }

    lv_obj_set_style_text_color(TopLabel, grey, 0);
    lv_obj_set_style_text_color(BottomUnits, lv_color_white(), 0);
    lv_obj_set_style_text_color(BottomLabel, grey, 0);
    lv_obj_set_style_text_color(clockDisplay, lv_color_white(), 0);
    lv_obj_set_style_text_color(BottomVariable, lv_color_white(), 0);
    lv_obj_set_style_text_color(AuxVariable, darkGrey, 0);

    updateDisplayInterval = 2000; // Start with very low screen refresh until gps fix
}
