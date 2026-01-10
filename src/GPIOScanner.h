#pragma once
#include <Arduino.h>

// GPIO Scanner utility to find the capacitive button
class GPIOScanner
{
private:
    // Common GPIO pins that could be used for buttons on ESP32-S3
    static const int CANDIDATE_PINS[];
    static const int NUM_CANDIDATES;

    struct PinState
    {
        int pin;
        bool lastState;
        uint32_t lastChange;
        uint32_t changeCount;
    };

    PinState pinStates[20]; // Max pins to monitor
    int numPins;

public:
    GPIOScanner();
    void begin();
    void scan();
    void printResults();
    bool hasStateChange(int pin);
};