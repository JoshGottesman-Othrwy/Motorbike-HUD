#include "display.h"

// External font declarations
LV_FONT_DECLARE(RobotoBlack_60);
LV_FONT_DECLARE(RobotoBlack_200);

Display::Display() {
    // Constructor
}

bool Display::begin() {
    Serial.print("Initializing display... ");

    // Automatically determine the access device
    bool rslt = amoled.begin();
    if (!rslt) {
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
    
    // Create UI elements
    createUIElements();
    
    // Run startup screen
    startUpScreen();
    
    return true;
}

void Display::initializeColors() {
    grey = lv_color_make(0xAA, 0xAA, 0xAA);
    darkGrey = lv_color_make(0x5C, 0x5C, 0x5C);
    veryDarkGrey = lv_color_make(0x32, 0x32, 0x32);
    green = lv_color_make(0x4C, 0xAF, 0x50);
    yellow = lv_color_make(0xFF, 0xEB, 0x3B);
    orange = lv_color_make(0xFF, 0x98, 0x00);
    red = lv_color_make(0xAA, 0x43, 0x36);
}

void Display::createUIElements() {
    // Set label formats/positions
    TopLabel = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(TopLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(TopLabel, lv_color_white(), 0);
    lv_obj_align(TopLabel, LV_ALIGN_TOP_LEFT, 5, 5);

    BottomLabel = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(BottomLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(BottomLabel, lv_color_white(), 0);
    lv_obj_align(BottomLabel, LV_ALIGN_TOP_LEFT, 5, 125);

    AuxVariable = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(AuxVariable, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(AuxVariable, grey, 0);
    lv_obj_align(AuxVariable, LV_ALIGN_TOP_LEFT, 105, 5);

    TopUnits = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(TopUnits, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(TopUnits, lv_color_white(), 0);
    lv_obj_align(TopUnits, LV_ALIGN_TOP_LEFT, 168, 65);

    BottomUnits = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(BottomUnits, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(BottomUnits, lv_color_white(), 0);
    lv_obj_align(BottomUnits, LV_ALIGN_TOP_LEFT, 168, 185);

    MainUnits = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(MainUnits, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(MainUnits, grey, 0);
    lv_obj_align(MainUnits, LV_ALIGN_BOTTOM_RIGHT, 0, -10);

    MainVariable = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(MainVariable, &RobotoBlack_200, 0);
    lv_obj_set_style_text_color(MainVariable, lv_color_white(), 0);
    lv_obj_align(MainVariable, LV_ALIGN_BOTTOM_RIGHT, 0, -25);

    TopVariable = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(TopVariable, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(TopVariable, lv_color_white(), 0);
    lv_obj_align(TopVariable, LV_ALIGN_BOTTOM_RIGHT, -370, -130);

    BottomVariable = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(BottomVariable, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(BottomVariable, lv_color_white(), 0);
    lv_obj_align(BottomVariable, LV_ALIGN_BOTTOM_RIGHT, -370, -10);

    debugDisp = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(debugDisp, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(debugDisp, green, 0);
    lv_obj_align(debugDisp, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_clear_flag(debugDisp, LV_OBJ_FLAG_HIDDEN);

    // Create Static Labels
    lv_label_set_text_fmt(TopLabel, "Sats.");
    lv_label_set_text_fmt(BottomLabel, "Recent Max.");
    lv_label_set_text_fmt(AuxVariable, "(Low)");
    lv_label_set_text_fmt(TopUnits, "s (0-60)");
    lv_label_set_text_fmt(BottomUnits, "mph");
    lv_label_set_text_fmt(MainUnits, "mph");
}

void Display::loop() {
    // Handle display updates in the main loop
    lv_task_handler();
}

void Display::updateGPSData(float speed, float speedMax, float hdop, int sats, float zeroToSixtyTime) {
    // Update Speed Animation
    if (millis() - prevSpeedUpdate > updateSpeed) {
        updateSpeedAnimation(speed);
        prevSpeedUpdate = millis();
    }

    // Update Display
    if (millis() - prevDispUpdate > updateDisplayInterval) {
        bool shouldFade = speedInt >= 100;
        if (shouldFade != isFaded) {
            lv_color_t color = shouldFade ? darkGrey : lv_color_white();
            lv_color_t labelColor = shouldFade ? darkGrey : grey;

            lv_obj_set_style_text_color(TopLabel, labelColor, 0);
            lv_obj_set_style_text_color(TopUnits, color, 0);
            lv_obj_set_style_text_color(BottomUnits, color, 0);
            lv_obj_set_style_text_color(BottomLabel, labelColor, 0);
            lv_obj_set_style_text_color(TopVariable, color, 0);
            lv_obj_set_style_text_color(BottomVariable, color, 0);
            lv_obj_set_style_text_color(AuxVariable, color, 0);

            isFaded = shouldFade;
        }

        lv_label_set_text_fmt(BottomVariable, "%.1f", speedMax);
        lv_label_set_text_fmt(TopLabel, "Sats. %u", sats);

        int hdopState;
        lv_color_t hdopStateColor;
        const char *hdopStateLabel;

        if (hdop < 1) {
            hdopState = 0;
            hdopStateColor = green;
            hdopStateLabel = " (Excellent)";
        } else if (hdop < 2) {
            hdopState = 1;
            hdopStateColor = green;
            hdopStateLabel = " (Good)";
        } else if (hdop < 5) {
            hdopState = 2;
            hdopStateColor = yellow;
            hdopStateLabel = " (Moderate)";
        } else if (hdop < 10) {
            hdopState = 3;
            hdopStateColor = orange;
            hdopStateLabel = " (Fair)";
        } else {
            hdopState = 4;
            hdopStateColor = red;
            hdopStateLabel = "(No Fix)";
        }

        if (hdopState != lastHdopState) {
            lv_obj_set_style_text_color(AuxVariable, hdopStateColor, 0);
            lv_label_set_text_fmt(AuxVariable, "%s", hdopStateLabel);
            lastHdopState = hdopState;
        }

        lv_label_set_text_fmt(TopVariable, "%.2f", zeroToSixtyTime);

        if (debug == 1) {
            lv_label_set_text_fmt(debugDisp, ".");
        } else {
            lv_label_set_text_fmt(debugDisp, " ");
        }
        debug = !debug;

        prevDispUpdate = millis();
    }
}

void Display::updateSpeedAnimation(float targetSpeed) {
    if (speedInt < round(targetSpeed)) {
        speedInt++;
        lv_label_set_text_fmt(MainVariable, "%u", speedInt);
    } else if (speedInt > round(targetSpeed)) {
        speedInt--;
        lv_label_set_text_fmt(MainVariable, "%u", speedInt);
    }
}

void Display::setFirstFix(bool hasFix) {
    updateDisplayInterval = hasFix ? 33 : 2000; // update display frequency on fix
}

void Display::setBrightness(uint8_t brightness) {
    amoled.setBrightness(brightness);
}

void Display::startUpScreen() {
    Serial.println("Startup Screen");

    lv_obj_set_style_text_color(TopLabel, darkGrey, 0);
    lv_obj_set_style_text_color(TopUnits, darkGrey, 0);
    lv_obj_set_style_text_color(BottomUnits, darkGrey, 0);
    lv_obj_set_style_text_color(BottomLabel, darkGrey, 0);
    lv_obj_set_style_text_color(TopVariable, darkGrey, 0);
    lv_obj_set_style_text_color(BottomVariable, darkGrey, 0);
    lv_obj_set_style_text_color(AuxVariable, darkGrey, 0);

    lv_label_set_text_fmt(BottomVariable, "0.00");
    lv_label_set_text_fmt(TopVariable, "0.00");

    for (int i = 0; i <= 100; i += 5) {
        lv_label_set_text_fmt(MainVariable, "%u", i);
        lv_task_handler();
        delay(10);
    }

    for (int i = 100; i >= 0; i -= 5) {
        lv_label_set_text_fmt(MainVariable, "%u", i);
        lv_task_handler();
        delay(10);
    }

    lv_obj_set_style_text_color(TopLabel, grey, 0);
    lv_obj_set_style_text_color(TopUnits, lv_color_white(), 0);
    lv_obj_set_style_text_color(BottomUnits, lv_color_white(), 0);
    lv_obj_set_style_text_color(BottomLabel, grey, 0);
    lv_obj_set_style_text_color(TopVariable, lv_color_white(), 0);
    lv_obj_set_style_text_color(BottomVariable, lv_color_white(), 0);
    lv_obj_set_style_text_color(AuxVariable, darkGrey, 0);

    updateDisplayInterval = 2000; // Start with very low screen refresh until gps fix
}
