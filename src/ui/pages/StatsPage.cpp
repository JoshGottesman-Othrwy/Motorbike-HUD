#include "StatsPage.h"

void StatsPage::create()
{
    // Sat status in top-left
    satsLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(satsLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(satsLabel, Theme::white(), 0);
    lv_obj_align(satsLabel, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_label_set_text(satsLabel, "Sats. 0");

    // Speed display at top center
    speedLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(speedLabel, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(speedLabel, Theme::white(), 0);
    lv_obj_align(speedLabel, LV_ALIGN_TOP_LEFT, 200, 5);
    lv_label_set_text(speedLabel, "0.0");

    // mph units label
    speedUnits = lv_label_create(tile);
    lv_obj_set_style_text_font(speedUnits, &lv_font_montserrat_34, 0);
    lv_obj_set_style_text_color(speedUnits, Theme::grey(), 0);
    lv_obj_align(speedUnits, LV_ALIGN_TOP_LEFT, 300, 15);
    lv_label_set_text(speedUnits, "mph");

    // Horizontal line under speed display
    lv_obj_t *headerLine = lv_line_create(tile);
    static lv_point_t headerLinePoints[] = {{0, 0}, {480, 0}};
    lv_line_set_points(headerLine, headerLinePoints, 2);
    lv_obj_set_style_line_color(headerLine, Theme::grey(), 0);
    lv_obj_set_style_line_width(headerLine, 4, 0);
    lv_obj_align(headerLine, LV_ALIGN_TOP_MID, 0, 70);

    // Vertical divider line splitting page in half
    lv_obj_t *dividerLine = lv_line_create(tile);
    static lv_point_t dividerLinePoints[] = {{0, 0}, {0, 120}};
    lv_line_set_points(dividerLine, dividerLinePoints, 2);
    lv_obj_set_style_line_color(dividerLine, Theme::grey(), 0);
    lv_obj_set_style_line_width(dividerLine, 2, 0);
    lv_obj_align(dividerLine, LV_ALIGN_TOP_MID, 0, 100);

    // 0-60 time display (left side below divider)
    zeroToSixtyLabel = lv_label_create(tile);
    lv_obj_set_style_text_font(zeroToSixtyLabel, &RobotoBlack_60, 0);
    lv_obj_set_style_text_color(zeroToSixtyLabel, Theme::white(), 0);
    lv_obj_align(zeroToSixtyLabel, LV_ALIGN_TOP_LEFT, 20, 87);
    lv_label_set_text(zeroToSixtyLabel, "0.00");

    // 0-60 units label
    zeroToSixtyUnits = lv_label_create(tile);
    lv_obj_set_style_text_font(zeroToSixtyUnits, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(zeroToSixtyUnits, Theme::grey(), 0);
    lv_obj_align(zeroToSixtyUnits, LV_ALIGN_TOP_LEFT, 150, 110);
    lv_label_set_text(zeroToSixtyUnits, "s (0-60)");
}

void StatsPage::update()
{
    // TODO: Add dynamic update logic here
    // This will be called periodically when this page is visible
}
