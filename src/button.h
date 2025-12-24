#pragma once
#include <Arduino.h>
#include <AceButton.h>

using namespace ace_button;

// Forward declarations
class LilyGo_AMOLED;
using LilyGo_Class = LilyGo_AMOLED;

class Button {
private:
    AceButton *buttons = nullptr;
    int buttonCount = 0;
    bool displayOn = true;
    uint8_t lastBrightness = 255;
    
    // Button event handler (static function for AceButton)
    static void buttonHandleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState);
    
    // Task handler for button checking
    static void buttonHandlerTask(void *ptr);
    
    // Reference to display for brightness control
    static Button* instance; // Static pointer to access instance in static methods
    
public:
    Button();
    bool begin(LilyGo_AMOLED &amoledRef);
    void loop();
    
    // Public methods for button functionality
    void toggleDisplay();
    void enterSleep();
    bool isDisplayOn() const { return displayOn; }
    
    // Getters
    bool wasPressed(); // Keep for compatibility
    
    // Reference to the AMOLED display
    LilyGo_AMOLED *amoled = nullptr;
};
