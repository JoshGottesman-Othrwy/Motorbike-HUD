#include "SpeedPage.h"

void SpeedPage::create()
{
    // Satellites label - top left
    satsLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(satsLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(satsLabel, Theme::white(), 0);
    lv_obj_align(satsLabel, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_label_set_text(satsLabel, "Sats. -");

    // GPS quality indicator
    gpsQualityLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(gpsQualityLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(gpsQualityLabel, Theme::grey(), 0);
    lv_obj_align(gpsQualityLabel, LV_ALIGN_TOP_LEFT, 105, 5);
    lv_label_set_text(gpsQualityLabel, "(No Fix)");

    // Recent max label
    recentMaxLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(recentMaxLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(recentMaxLabel, Theme::white(), 0);
    lv_obj_align(recentMaxLabel, LV_ALIGN_TOP_LEFT, 5, 125);
    lv_label_set_text(recentMaxLabel, "Recent Max.");

    // Recent max units
    recentMaxUnits = lv_label_create(tile);
    lv_obj_set_style_text_font(recentMaxUnits, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(recentMaxUnits, Theme::white(), 0);
    lv_obj_align(recentMaxUnits, LV_ALIGN_TOP_LEFT, 168, 185);
    lv_label_set_text(recentMaxUnits, "mph");

    // Main speed units
    mainSpeedUnits = lv_label_create(tile);
    lv_obj_set_style_text_font(mainSpeedUnits, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(mainSpeedUnits, Theme::grey(), 0);
    lv_obj_align(mainSpeedUnits, LV_ALIGN_BOTTOM_RIGHT, 0, -10);
    lv_label_set_text(mainSpeedUnits, "mph");

    // Main speed display (large number)
    mainSpeed = lv_label_create(tile);
    lv_obj_set_style_text_font(mainSpeed, &RobotoBlack_200, 0);
    lv_obj_set_style_text_color(mainSpeed, Theme::white(), 0);
    lv_obj_align(mainSpeed, LV_ALIGN_BOTTOM_RIGHT, 0, -25);
    lv_label_set_text(mainSpeed, "--");

    // Clock display
    clockDisplay = lv_label_create(tile);
    lv_obj_set_style_text_font(clockDisplay, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(clockDisplay, Theme::white(), 0);
    lv_obj_align(clockDisplay, LV_ALIGN_BOTTOM_RIGHT, -350, -130);
    lv_label_set_text(clockDisplay, "--:--");

    // Recent max speed value
    recentMaxValue = lv_label_create(tile);
    lv_obj_set_style_text_font(recentMaxValue, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(recentMaxValue, Theme::white(), 0);
    lv_obj_align(recentMaxValue, LV_ALIGN_BOTTOM_RIGHT, -370, -10);
    lv_label_set_text(recentMaxValue, "0.0");
}

void SpeedPage::update()
{
    // Update GPS displays using change detection for efficiency
    updateSatelliteDisplay();
    updateGPSStatusDisplay();
}

// ============================================================================
// SATELLITE COUNT DISPLAY
// Only updates the label if the value has changed
// ============================================================================
void SpeedPage::updateSatelliteDisplay()
{
    int32_t currentSats = gps.getSatelliteCount();

    // Skip update if value hasn't changed (efficiency)
    if (currentSats == cachedSatelliteCount)
    {
        return;
    }

    // Update cache and display
    cachedSatelliteCount = currentSats;

    // Format: "Sats. X" or "Sats. -" if no data
    char buffer[16];
    if (gps.isConnected())
    {
        snprintf(buffer, sizeof(buffer), "Sats. %ld", currentSats);
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "Sats. -");
    }

    lv_label_set_text(satsLabel, buffer);
}

// ============================================================================
// GPS STATUS DISPLAY
// Updates status text and color only when status changes
// Color coding: Red (error), Orange (poor), Yellow (fair), Green (good/excellent)
// ============================================================================
void SpeedPage::updateGPSStatusDisplay()
{
    GPSStatus currentStatus = gps.getStatus();

    // Skip update if status hasn't changed (efficiency)
    // But always update on first run to sync display with actual state
    if (currentStatus == cachedGPSStatus && !firstUpdate)
    {
        return;
    }

    // Update cache and clear first update flag
    cachedGPSStatus = currentStatus;
    firstUpdate = false;

    // Update text
    lv_label_set_text(gpsQualityLabel, getStatusText(currentStatus));

    // Update color based on status
    lv_obj_set_style_text_color(gpsQualityLabel, getStatusColor(currentStatus), 0);
}

// ============================================================================
// STATUS COLOR MAPPING
// Returns appropriate color for each GPS status level
// ============================================================================
lv_color_t SpeedPage::getStatusColor(GPSStatus status)
{
    switch (status)
    {
    case GPSStatus::NotConnected:
    case GPSStatus::NoFix:
        return lv_color_hex(0xFF0000); // Red - Error/No signal

    case GPSStatus::Poor:
        return lv_color_hex(0xFF8000); // Orange - Poor accuracy

    case GPSStatus::Fair:
        return lv_color_hex(0xFFFF00); // Yellow - Fair accuracy

    case GPSStatus::Good:
    case GPSStatus::Excellent:
        return lv_color_hex(0x00FF00); // Green - Good/Excellent accuracy

    default:
        return Theme::grey(); // Fallback
    }
}

// ============================================================================
// STATUS TEXT MAPPING
// Returns human-readable text for each GPS status
// ============================================================================
const char *SpeedPage::getStatusText(GPSStatus status)
{
    switch (status)
    {
    case GPSStatus::NotConnected:
        return "(No GPS)";

    case GPSStatus::NoFix:
        return "(No Fix)";

    case GPSStatus::Poor:
        return "(Poor)";

    case GPSStatus::Fair:
        return "(Fair)";

    case GPSStatus::Good:
        return "(Good)";

    case GPSStatus::Excellent:
        return "(Excellent)";

    default:
        return "(Unknown)";
    }
}
