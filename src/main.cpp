#include <Arduino.h>
#include "display.h"
#include "sensors/GPS.h"

// Create display instance
Display display;

// Create GPS instance
GPS gps;

// Task handles
TaskHandle_t displayTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;

// Mutex for protecting shared data between cores
SemaphoreHandle_t dataMutex = NULL;

// ============================================================================
// DISPLAY TASK - Runs on Core 1 (main core)
// Handles all LVGL rendering and UI updates
// ============================================================================
void displayTask(void *parameter)
{
    Serial.println("[Core 1] Display task started");

    for (;;)
    {
        // Call LVGL task handler for UI responsiveness
        lv_task_handler();

        // Update the current page (for any dynamic content)
        display.update();

        // Small delay - LVGL recommends 5-10ms between calls
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// ============================================================================
// SENSOR TASK - Runs on Core 0 (protocol core)
// Handles GPS, accelerometer, and other sensor polling
// ============================================================================
void sensorTask(void *parameter)
{
    Serial.println("[Core 0] Sensor task started");

    for (;;)
    {
        // Process GPS data
        gps.loop();

        // Add other sensor polling here as needed
        // Example: accelerometer.loop(), temperature.loop(), etc.

        // Sensor polling rate - adjust as needed
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup(void)
{
    delay(50);
    Serial.begin(115200);
    Serial.println("=======MOTO DISPLAY=======");
    Serial.printf("ESP32-S3 running at %d MHz\n", getCpuFrequencyMhz());

    // Create mutex for thread-safe data access
    dataMutex = xSemaphoreCreateMutex();

    // Initialize display (this also initializes all pages)
    // Must be done on Core 1 where LVGL will run
    bool displayOk = display.begin();
    if (!displayOk)
    {
        Serial.println("Display initialization failed!");
        while (1)
        {
            delay(1000);
        }
    }

    Serial.println("Display initialized successfully!");

    // Initialize GPS
    Serial.print("Initializing GPS... ");
    bool gpsOk = gps.begin();
    Serial.println(gpsOk ? "OK" : "FAILED");

    if (!gpsOk)
    {
        Serial.println("Warning: GPS initialization failed - continuing without GPS");
    }

    // Create display task on Core 1 (main core, higher priority)
    xTaskCreatePinnedToCore(
        displayTask,        // Task function
        "DisplayTask",      // Task name
        8192,               // Stack size (bytes)
        NULL,               // Parameters
        2,                  // Priority (higher = more important)
        &displayTaskHandle, // Task handle
        1                   // Core 1
    );

    // Create sensor task on Core 0 (protocol core, lower priority)
    xTaskCreatePinnedToCore(
        sensorTask,        // Task function
        "SensorTask",      // Task name
        4096,              // Stack size (bytes)
        NULL,              // Parameters
        1,                 // Priority
        &sensorTaskHandle, // Task handle
        0                  // Core 0
    );

    Serial.println("Dual-core tasks created:");
    Serial.println("  - Core 1: Display/LVGL (priority 2)");
    Serial.println("  - Core 0: Sensors/GPS (priority 1)");
    Serial.println("System ready - swipe to navigate between pages");
}

void loop()
{
    // Main loop is now empty - all work done in dedicated tasks
    // Just yield to let other tasks run
    vTaskDelay(pdMS_TO_TICKS(1000));
}
