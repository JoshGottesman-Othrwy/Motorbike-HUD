#include "InfoPage.h"
#include <Arduino.h>

void InfoPage::create()
{
    // Make info page scrollable
    lv_obj_set_scrollbar_mode(tile, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(tile, LV_DIR_VER);
    lv_obj_set_style_pad_all(tile, 0, 0);

    // Page title
    lv_obj_t *infoTitle = lv_label_create(tile);
    lv_obj_set_style_text_font(infoTitle, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(infoTitle, Theme::white(), 0);
    lv_label_set_text(infoTitle, "INFO");
    lv_obj_align(infoTitle, LV_ALIGN_TOP_MID, 0, 20);

    // WiFi Status section header
    lv_obj_t *wifiHeader = lv_label_create(tile);
    lv_obj_set_style_text_font(wifiHeader, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(wifiHeader, Theme::grey(), 0);
    lv_label_set_text(wifiHeader, "WiFi Status: ");
    lv_obj_align(wifiHeader, LV_ALIGN_TOP_LEFT, 10, 70);

    // WiFi status value
    wifiStatusLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(wifiStatusLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(wifiStatusLabel, Theme::red(), 0);
    lv_label_set_text(wifiStatusLabel, "Disconnected");
    lv_obj_align(wifiStatusLabel, LV_ALIGN_TOP_LEFT, 190, 70);

    // SSID label
    wifiSSIDLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(wifiSSIDLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(wifiSSIDLabel, Theme::grey(), 0);
    lv_obj_set_width(wifiSSIDLabel, 430);
    lv_label_set_long_mode(wifiSSIDLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(wifiSSIDLabel, "SSID: ---");
    lv_obj_align(wifiSSIDLabel, LV_ALIGN_TOP_LEFT, 10, 110);

    // IP address label
    wifiIPLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(wifiIPLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(wifiIPLabel, Theme::grey(), 0);
    lv_obj_set_width(wifiIPLabel, 430);
    lv_label_set_long_mode(wifiIPLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(wifiIPLabel, "IP: 0.0.0.0");
    lv_obj_align(wifiIPLabel, LV_ALIGN_TOP_LEFT, 10, 140);

    // Module Status section header
    lv_obj_t *moduleHeader = lv_label_create(tile);
    lv_obj_set_style_text_font(moduleHeader, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(moduleHeader, Theme::grey(), 0);
    lv_label_set_text(moduleHeader, "Module Status:");
    lv_obj_align(moduleHeader, LV_ALIGN_TOP_LEFT, 10, 200);

    // GPS status
    moduleGPSLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(moduleGPSLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(moduleGPSLabel, Theme::red(), 0);
    lv_label_set_text(moduleGPSLabel, "GPS: Not detected");
    lv_obj_align(moduleGPSLabel, LV_ALIGN_TOP_LEFT, 10, 240);

    // Magnetometer status
    moduleMagnetometerLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(moduleMagnetometerLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(moduleMagnetometerLabel, Theme::red(), 0);
    lv_label_set_text(moduleMagnetometerLabel, "Magnetometer: Not detected");
    lv_obj_align(moduleMagnetometerLabel, LV_ALIGN_TOP_LEFT, 10, 270);

    // IMU status
    moduleIMULabel = lv_label_create(tile);
    lv_obj_set_style_text_font(moduleIMULabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(moduleIMULabel, Theme::red(), 0);
    lv_label_set_text(moduleIMULabel, "IMU: Not detected");
    lv_obj_align(moduleIMULabel, LV_ALIGN_TOP_LEFT, 10, 300);

    // Battery Status section header
    lv_obj_t *batteryHeader = lv_label_create(tile);
    lv_obj_set_style_text_font(batteryHeader, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(batteryHeader, Theme::grey(), 0);
    lv_label_set_text(batteryHeader, "Battery Status:");
    lv_obj_align(batteryHeader, LV_ALIGN_TOP_LEFT, 10, 340);

    // Battery voltage
    batteryVoltageLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(batteryVoltageLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(batteryVoltageLabel, Theme::white(), 0);
    lv_label_set_text(batteryVoltageLabel, "Voltage: --.-V");
    lv_obj_align(batteryVoltageLabel, LV_ALIGN_TOP_LEFT, 10, 380);

    // Battery status (charging/discharging)
    batteryStatusLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(batteryStatusLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(batteryStatusLabel, Theme::white(), 0);
    lv_label_set_text(batteryStatusLabel, "Status: Unknown");
    lv_obj_align(batteryStatusLabel, LV_ALIGN_TOP_LEFT, 10, 410);

    // Battery percentage estimate
    batteryPercentLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(batteryPercentLabel, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(batteryPercentLabel, Theme::white(), 0);
    lv_label_set_text(batteryPercentLabel, "Charge: --%");
    lv_obj_align(batteryPercentLabel, LV_ALIGN_TOP_LEFT, 10, 440);

    // Debug section header (moved down)
    lv_obj_t *debugHeader = lv_label_create(tile);
    lv_obj_set_style_text_font(debugHeader, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(debugHeader, Theme::yellow(), 0);
    lv_label_set_text(debugHeader, "Debug:");
    lv_obj_align(debugHeader, LV_ALIGN_TOP_LEFT, 10, 500);

    // Frame counter (moved down)
    debugFrameCounter = lv_label_create(tile);
    lv_obj_set_style_text_font(debugFrameCounter, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(debugFrameCounter, Theme::grey(), 0);
    lv_label_set_text(debugFrameCounter, "Frames: 0");
    lv_obj_align(debugFrameCounter, LV_ALIGN_TOP_LEFT, 10, 540);

    // FPS display (moved down)
    debugFPS = lv_label_create(tile);
    lv_obj_set_style_text_font(debugFPS, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(debugFPS, Theme::grey(), 0);
    lv_label_set_text(debugFPS, "FPS: 0.0");
    lv_obj_align(debugFPS, LV_ALIGN_TOP_LEFT, 10, 570);

    // Uptime display (moved down)
    debugUptime = lv_label_create(tile);
    lv_obj_set_style_text_font(debugUptime, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(debugUptime, Theme::grey(), 0);
    lv_label_set_text(debugUptime, "Uptime: 0s");
    lv_obj_align(debugUptime, LV_ALIGN_TOP_LEFT, 10, 600);
}

void InfoPage::update()
{
    // Increment frame counter
    frameCount++;
    framesThisSecond++;

    // Calculate FPS every second
    uint32_t now = millis();
    if (now - lastFPSUpdate >= 1000)
    {
        currentFPS = framesThisSecond * 1000.0f / (now - lastFPSUpdate);
        framesThisSecond = 0;
        lastFPSUpdate = now;
    }

    // Update GPS module status
    if (moduleGPSLabel)
    {
        if (gps.isConnected())
        {
            GPSStatus status = gps.getStatus();
            if (status == GPSStatus::NoFix)
            {
                lv_label_set_text(moduleGPSLabel, "GPS: Connected (No Fix)");
                lv_obj_set_style_text_color(moduleGPSLabel, Theme::yellow(), 0);
            }
            else
            {
                // Has fix - show satellite count
                lv_label_set_text_fmt(moduleGPSLabel, "GPS: %ld sats (%s)",
                                      gps.getSatelliteCount(), gps.getStatusString());
                lv_obj_set_style_text_color(moduleGPSLabel, Theme::green(), 0);
            }
        }
        else
        {
            lv_label_set_text(moduleGPSLabel, "GPS: Not detected");
            lv_obj_set_style_text_color(moduleGPSLabel, Theme::red(), 0);
        }
    }

    // Update Magnetometer status (not yet implemented)
    if (moduleMagnetometerLabel)
    {
        // TODO: Update when magnetometer sensor is added
        lv_label_set_text(moduleMagnetometerLabel, "Magnetometer: N/A");
        lv_obj_set_style_text_color(moduleMagnetometerLabel, Theme::grey(), 0);
    }

    // Update IMU status (not yet implemented)
    if (moduleIMULabel)
    {
        // TODO: Update when IMU sensor is added
        lv_label_set_text(moduleIMULabel, "IMU: N/A");
        lv_obj_set_style_text_color(moduleIMULabel, Theme::grey(), 0);
    }

    // Update battery status
    LilyGo_AMOLED &amoled = display.getAmoled();

    // Battery voltage
    if (batteryVoltageLabel)
    {
        uint16_t voltage = amoled.getBattVoltage();
        if (voltage > 0)
        {
            lv_label_set_text_fmt(batteryVoltageLabel, "Voltage: %.2fV", voltage / 1000.0f);
        }
        else
        {
            lv_label_set_text(batteryVoltageLabel, "Voltage: No Battery");
        }
    }

    // Charging status
    if (batteryStatusLabel)
    {
        uint16_t battVoltage = amoled.getBattVoltage();
        bool usbConnected = amoled.isVbusIn();
        
        if (battVoltage == 0)
        {
            lv_label_set_text(batteryStatusLabel, "Status: No Battery");
            lv_obj_set_style_text_color(batteryStatusLabel, Theme::red(), 0);
        }
        else if (usbConnected)
        {
            lv_label_set_text(batteryStatusLabel, "Status: Charging");
            lv_obj_set_style_text_color(batteryStatusLabel, Theme::green(), 0);
        }
        else
        {
            lv_label_set_text(batteryStatusLabel, "Status: Discharging");
            lv_obj_set_style_text_color(batteryStatusLabel, Theme::yellow(), 0);
        }
    }

    // Battery percentage estimate (rough Li-ion estimation)
    if (batteryPercentLabel)
    {
        uint16_t voltage = amoled.getBattVoltage();
        if (voltage > 0)
        {
        float voltageFloat = voltage / 1000.0f;

        // Simple Li-ion percentage estimation (3.0V = 0%, 4.2V = 100%)
        int percentage = 0;
        if (voltageFloat >= 4.2f)
        {
            percentage = 100;
        }
        else if (voltageFloat >= 3.0f)
        {
            percentage = (int)((voltageFloat - 3.0f) / 1.2f * 100.0f);
        }

        lv_label_set_text_fmt(batteryPercentLabel, "Charge: %d%%", percentage);

        // Color code based on percentage
        if (percentage > 50)
        {
            lv_obj_set_style_text_color(batteryPercentLabel, Theme::green(), 0);
        }
        else if (percentage > 20)
        {
            lv_obj_set_style_text_color(batteryPercentLabel, Theme::yellow(), 0);
        }
        else
        {
            lv_obj_set_style_text_color(batteryPercentLabel, Theme::red(), 0);
        }
        }
        else
        {
            lv_label_set_text(batteryPercentLabel, "Charge: --");
            lv_obj_set_style_text_color(batteryPercentLabel, Theme::grey(), 0);
        }
    }

    // Update frame counter display
    if (debugFrameCounter)
    {
        lv_label_set_text_fmt(debugFrameCounter, "Frames: %lu", frameCount);
    }

    // Update FPS display
    if (debugFPS)
    {
        lv_label_set_text_fmt(debugFPS, "FPS: %.1f", currentFPS);
    }

    // Update uptime display
    if (debugUptime)
    {
        uint32_t uptime = now / 1000;
        uint32_t hours = uptime / 3600;
        uint32_t minutes = (uptime % 3600) / 60;
        uint32_t seconds = uptime % 60;
        lv_label_set_text_fmt(debugUptime, "Uptime: %02lu:%02lu:%02lu", hours, minutes, seconds);
    }
}
