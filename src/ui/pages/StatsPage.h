#pragma once
#include "../Page.h"
#include "../Theme.h"

// External font declarations
LV_FONT_DECLARE(RobotoBlack_60);

/**
 * Driving statistics page.
 * Shows satellite count, current speed, and 0-60 time.
 */
class StatsPage : public Page
{
private:
    // UI Elements
    lv_obj_t *satsLabel = nullptr;
    lv_obj_t *speedLabel = nullptr;
    lv_obj_t *speedUnits = nullptr;
    lv_obj_t *zeroToSixtyLabel = nullptr;
    lv_obj_t *zeroToSixtyUnits = nullptr;

public:
    StatsPage() : Page("Stats") {}

    void create() override;
    void update() override;
};
