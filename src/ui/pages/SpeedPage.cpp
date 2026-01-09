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
    // Only update when page is active for efficiency
    if (!isPageActive)
    {
        return;
    }

    // Update GPS displays using change detection for efficiency
    updateSatelliteDisplay();
    updateGPSStatusDisplay();
    updateClockDisplay();
    updateSpeedDisplay();
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

// ============================================================================
// CLOCK DISPLAY UPDATE
// Shows GPS time when available, "--:--" when no GPS time
// Only updates when time changes for efficiency
// ============================================================================
void SpeedPage::updateClockDisplay()
{
    GPSTime currentTime = gps.getTime();
    bool timeIsValid = currentTime.valid && gps.isConnected();

    // Check if we need to update
    bool timeChanged = (currentTime.hour != cachedHour || currentTime.minute != cachedMinute);
    bool timeValidityChanged = (timeIsValid != cachedTimeWasValid);

    if (!timeChanged && !timeValidityChanged && !firstUpdate)
    {
        return; // No change, skip update for efficiency
    }

    // Update cache
    cachedHour = currentTime.hour;
    cachedMinute = currentTime.minute;
    cachedTimeWasValid = timeIsValid;

    // Format and display time
    char timeBuffer[8];
    if (timeIsValid)
    {
        // Format as HH:MM (24-hour format)
        snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", currentTime.hour, currentTime.minute);
    }
    else
    {
        // No valid GPS time
        snprintf(timeBuffer, sizeof(timeBuffer), "--:--");
    }

    lv_label_set_text(clockDisplay, timeBuffer);
}

// ============================================================================
// SPEED DISPLAY UPDATE
// Shows GPS speed when available, "--" when no GPS fix
// Rounds to nearest whole number, clamps values under 0.8 to zero
// Only updates when speed changes for efficiency
// ============================================================================
void SpeedPage::updateSpeedDisplay()
{
    // Get raw speed from GPS
    float rawSpeed = gps.getSpeedMph();

    // Apply clamping and rounding logic to get target speed
    int32_t newTargetSpeed;
    if (!gps.hasFix() || !gps.isConnected())
    {
        newTargetSpeed = -1; // Use -1 to indicate no data (will show "--")
    }
    else if (rawSpeed < 0.8f)
    {
        newTargetSpeed = 0; // Clamp low speeds to zero
    }
    else
    {
        newTargetSpeed = static_cast<int32_t>(rawSpeed + 0.5f); // Round to nearest whole number
    }

    // Update target if it has changed
    if (newTargetSpeed != targetSpeed)
    {
        targetSpeed = newTargetSpeed;

        // If no GPS data, update display immediately
        if (targetSpeed == -1)
        {
            displayedSpeed = -1;
            lv_label_set_text(mainSpeed, "--");
            return;
        }

        // If first update or large jump (more than 10 mph), set immediately
        if (firstUpdate || abs(targetSpeed - displayedSpeed) > 10)
        {
            displayedSpeed = targetSpeed;
            lv_label_set_text_fmt(mainSpeed, "%ld", displayedSpeed);
            return;
        }
    }

    // Smooth animation: increment displayed speed towards target
    uint32_t now = millis();
    if (displayedSpeed != targetSpeed && now - lastSpeedUpdate >= SPEED_INCREMENT_INTERVAL_MS)
    {
        if (displayedSpeed < targetSpeed)
        {
            displayedSpeed++;
        }
        else if (displayedSpeed > targetSpeed)
        {
            displayedSpeed--;
        }

        // Update display
        lv_label_set_text_fmt(mainSpeed, "%ld", displayedSpeed);
        lastSpeedUpdate = now;
    }
}
