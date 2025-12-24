#include "button.h"
#include "LilyGo_AMOLED.h"

// Static member initialization
Button* Button::instance = nullptr;

Button::Button() {
    instance = this; // Set static instance pointer
}

bool Button::begin(LilyGo_AMOLED &amoledRef) {
    Serial.print("Initializing Buttons... ");
    
    amoled = &amoledRef;
    const BoardsConfigure_t *boards = amoled->getBoardsConfigure();
    
    if (boards->buttonNum > 0) {
        buttonCount = boards->buttonNum;
        buttons = new AceButton[buttonCount];
        
        ButtonConfig *buttonConfig;
        for (int i = 0; i < buttonCount; ++i) {
            Serial.print("Init button: ");
            Serial.println(i);
            
            pinMode(boards->pButtons[i], INPUT_PULLUP);
            buttons[i].init(boards->pButtons[i], HIGH, i);
            buttonConfig = buttons[i].getButtonConfig();
            buttonConfig->setFeature(ButtonConfig::kFeatureClick);
            buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
            buttonConfig->setEventHandler(buttonHandleEvent);
        }
        
        // Create button handler task
        xTaskCreate(buttonHandlerTask, "btn", 5 * 1024, NULL, 12, NULL);
        
        Serial.println("OK");
    } else {
        Serial.println("No buttons found");
    }
    
    // Set up home button callback for capacitive touch
    amoled->setHomeButtonCallback([](void *ptr) {
        Serial.println("Home key pressed!");
        if (Button::instance) {
            Button::instance->toggleDisplay();
        }
    }, NULL);
    
    return true;
}

void Button::loop() {
    // Button checking is handled by the FreeRTOS task
    // This method is kept for compatibility but doesn't need to do anything
}

void Button::buttonHandleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState) {
    if (!instance || !instance->amoled) return;
    
    const BoardsConfigure_t *boards = instance->amoled->getBoardsConfigure();
    uint8_t id = instance->amoled->getBoardID();
    
    switch (eventType) {
    case AceButton::kEventClicked:
        Serial.println("Button clicked!");
        instance->toggleDisplay();
        break;
        
    case AceButton::kEventLongPressed:
        Serial.println("Button long pressed - entering sleep!");
        instance->enterSleep();
        break;
        
    default:
        break;
    }
}

void Button::buttonHandlerTask(void *ptr) {
    if (!instance || !instance->amoled) {
        vTaskDelete(NULL);
        return;
    }
    
    const BoardsConfigure_t *boards = instance->amoled->getBoardsConfigure();
    
    while (1) {
        for (int i = 0; i < boards->buttonNum; ++i) {
            instance->buttons[i].check();
        }
        delay(5);
    }
    vTaskDelete(NULL);
}

void Button::toggleDisplay() {
    static uint32_t checkMs = 0;
    
    if (millis() > checkMs) {
        if (amoled->getBrightness()) {
            lastBrightness = amoled->getBrightness();
            amoled->setBrightness(0);
            displayOn = false;
            Serial.println("Display OFF");
        } else {
            amoled->setBrightness(lastBrightness);
            displayOn = true;
            Serial.println("Display ON");
        }
    }
    checkMs = millis() + 200; // Debounce
}

void Button::enterSleep() {
    Serial.println("Entering sleep mode...");
    
    const BoardsConfigure_t *boards = amoled->getBoardsConfigure();
    uint8_t id = amoled->getBoardID();
    
    // Disconnect WiFi if connected
    // WiFi.disconnect();
    // WiFi.mode(WIFI_OFF);
    
    bool touchpad_sleep = true;
    if (id == LILYGO_AMOLED_191 || id == LILYGO_AMOLED_191_SPI) {
        // 1.91 inch touch screen cannot wake up from sleep
        touchpad_sleep = false;
    }
    
    amoled->sleep(touchpad_sleep);
    
    if (boards->pmu && id == LILYGO_AMOLED_147) {
        // Set PMU Sleep mode for 1.47 inch
        amoled->enableSleep();
        amoled->clearPMU();
        amoled->enableWakeup();
        esp_sleep_enable_timer_wakeup(60 * 1000000ULL);  // 60S wakeup
    } else {
        // Set BOOT button as wakeup source for other models
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, LOW);
    }
    
    Wire.end();
    
    Serial.println("Sleep starting in 3 seconds...");
    delay(3000);
    esp_deep_sleep_start();
}

bool Button::wasPressed() {
    // This is kept for compatibility with existing code
    // The actual button handling is done through AceButton events
    static bool lastPressed = false;
    bool currentPressed = !displayOn; // Simple state for compatibility
    bool result = (currentPressed && !lastPressed);
    lastPressed = currentPressed;
    return result;
}
