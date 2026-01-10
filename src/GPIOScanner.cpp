#include "GPIOScanner.h"
#include <Arduino.h>

// Common GPIO pins that could be buttons (excluding known used pins)
const int GPIOScanner::CANDIDATE_PINS[] = {
    // Common button pins on ESP32-S3 boards
    21, // Often used for buttons
    47, // Available GPIO
    48, // Available GPIO
    45, // Available GPIO
    46, // Available GPIO
    9,  // Available GPIO
    10, // Available GPIO
    11, // Available GPIO
    12, // Available GPIO
    13, // Available GPIO (but might be touch IRQ)
    14, // Available GPIO (but might be touch RST)
    15, // Available GPIO
    16, // Available GPIO
    17, // Available GPIO
    18, // Available GPIO (but might be display)
    8,  // Available GPIO (but might be sensor IRQ)
    42, // Available GPIO
    41, // Available GPIO
    40, // Available GPIO
    39  // Available GPIO
};

const int GPIOScanner::NUM_CANDIDATES = sizeof(CANDIDATE_PINS) / sizeof(CANDIDATE_PINS[0]);

GPIOScanner::GPIOScanner() : numPins(0) {}

void GPIOScanner::begin()
{
    Serial.println("\\n=== GPIO Scanner Starting ===");
    Serial.println("This will monitor GPIO pins for button presses.");
    Serial.println("Press the capacitive button and watch for pin changes...");

    numPins = 0;
    for (int i = 0; i < NUM_CANDIDATES && numPins < 20; i++)
    {
        int pin = CANDIDATE_PINS[i];

        // Skip pins that are known to be used by display/I2C
        if (pin == 1 || pin == 2 || pin == 3 || // I2C pins
            pin == 4 || pin == 5 || pin == 6 || pin == 7)
        { // Display pins
            continue;
        }

        pinMode(pin, INPUT_PULLUP);

        pinStates[numPins].pin = pin;
        pinStates[numPins].lastState = digitalRead(pin);
        pinStates[numPins].lastChange = millis();
        pinStates[numPins].changeCount = 0;

        numPins++;
        Serial.printf("Monitoring GPIO%d\\n", pin);
    }

    Serial.printf("Monitoring %d GPIO pins for changes...\\n\\n", numPins);
}

void GPIOScanner::scan()
{
    uint32_t now = millis();

    for (int i = 0; i < numPins; i++)
    {
        bool currentState = digitalRead(pinStates[i].pin);

        if (currentState != pinStates[i].lastState)
        {
            pinStates[i].lastState = currentState;
            pinStates[i].lastChange = now;
            pinStates[i].changeCount++;

            Serial.printf("*** GPIO%d changed to %s ***\\n",
                          pinStates[i].pin,
                          currentState ? "HIGH" : "LOW");
        }
    }
}

void GPIOScanner::printResults()
{
    Serial.println("\\n=== GPIO Scanner Results ===");

    bool foundChanges = false;
    for (int i = 0; i < numPins; i++)
    {
        if (pinStates[i].changeCount > 0)
        {
            Serial.printf("GPIO%d: %d changes (current: %s)\\n",
                          pinStates[i].pin,
                          pinStates[i].changeCount,
                          pinStates[i].lastState ? "HIGH" : "LOW");
            foundChanges = true;
        }
    }

    if (!foundChanges)
    {
        Serial.println("No pin changes detected.");
    }
    Serial.println("===============================\\n");
}

bool GPIOScanner::hasStateChange(int pin)
{
    for (int i = 0; i < numPins; i++)
    {
        if (pinStates[i].pin == pin)
        {
            return pinStates[i].changeCount > 0;
        }
    }
    return false;
}