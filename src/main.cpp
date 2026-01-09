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

    const TickType_t targetInterval = pdMS_TO_TICKS(5); // 5ms target interval
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        TickType_t startTime = xTaskGetTickCount();

        // Call LVGL task handler for UI responsiveness
        lv_task_handler();

        // Update the current page (for any dynamic content)
        display.update();

        // Calculate elapsed time and delay only if needed
        TickType_t elapsedTime = xTaskGetTickCount() - startTime;

        if (elapsedTime < targetInterval)
        {
            // Only delay for remaining time if we finished early
            vTaskDelay(targetInterval - elapsedTime);
        }
        // If processing took >= 5ms, continue immediately (no extra delay)
    }
}

// ============================================================================
// SENSOR TASK - Runs on Core 0 (protocol core)
// Handles GPS, accelerometer, and other sensor polling
// ============================================================================
void sensorTask(void *parameter)
{
    Serial.println("[Core 0] Sensor task started");

    // GPS status monitoring variables
    GPSStatus lastGPSStatus = GPSStatus::NotConnected;
    uint32_t lastSatCount = 0;
    uint32_t statusUpdateInterval = 5000; // Print status every 5 seconds
    uint32_t lastStatusPrint = 0;
    uint32_t nmeaCharCount = 0;
    uint32_t lastNmeaCount = 0;

    // Low voltage monitoring variables
    uint32_t lastVoltageCheck = 0;
    uint32_t voltageCheckInterval = 30000; // Check every 30 seconds
    bool lowVoltageWarning = false;

    for (;;)
    {
        uint32_t now = millis();

        // Check battery voltage periodically for early warning
        if (now - lastVoltageCheck >= voltageCheckInterval)
        {
            uint16_t battVoltage = display.getAmoled().getBattVoltage();
            bool isCharging = display.getAmoled().isVbusIn();

            // Only check voltage when not charging (charging voltage is always 4.2V)
            if (battVoltage > 0 && !isCharging)
            {
                float voltage = battVoltage / 1000.0f;

                // Critical voltage - force shutdown (3.2V for Li-ion safety)
                if (voltage < 3.2f)
                {
                    Serial.printf("CRITICAL: Battery voltage %.2fV - forcing shutdown!\n", voltage);
                    display.getAmoled().shutdown();
                }
                // Warning voltage (3.4V)
                else if (voltage < 3.4f && !lowVoltageWarning)
                {
                    Serial.printf("WARNING: Low battery voltage %.2fV - connect charger soon!\n", voltage);
                    lowVoltageWarning = true;
                }
                // Clear warning when voltage recovers
                else if (voltage >= 3.6f && lowVoltageWarning)
                {
                    Serial.printf("Battery voltage recovered: %.2fV\n", voltage);
                    lowVoltageWarning = false;
                }
            }
            // Reset warning when charging
            else if (isCharging && lowVoltageWarning)
            {
                Serial.println("Battery charging - low voltage warning cleared");
                lowVoltageWarning = false;
            }

            lastVoltageCheck = now;
        }

        // Process GPS data (this also reads from Serial1)
        gps.loop();

        // Get NMEA character count from GPS class for debugging
        nmeaCharCount = gps.getCharsProcessed();

        // Monitor GPS status changes
        GPSStatus currentStatus = gps.getStatus();
        uint32_t currentSats = gps.getSatelliteCount();

        // Print status when it changes (fixed the duplicate string issue)
        if (currentStatus != lastGPSStatus)
        {
            const char *oldStatusStr = "";
            switch (lastGPSStatus)
            {
            case GPSStatus::NotConnected:
                oldStatusStr = "NC";
                break;
            case GPSStatus::NoFix:
                oldStatusStr = "No Fix";
                break;
            case GPSStatus::Poor:
                oldStatusStr = "Poor";
                break;
            case GPSStatus::Fair:
                oldStatusStr = "Fair";
                break;
            case GPSStatus::Good:
                oldStatusStr = "Good";
                break;
            case GPSStatus::Excellent:
                oldStatusStr = "Excellent";
                break;
            }

            Serial.printf("[GPS] Status changed: %s -> %s\n",
                          oldStatusStr, gps.getStatusString());

            // Print helpful message when status changes
            if (currentStatus == GPSStatus::NoFix && gps.isConnected())
            {
                Serial.println("[GPS] Module connected but no satellite fix (normal indoors)");
            }

            lastGPSStatus = currentStatus;
        }

        // // Print satellite count when it changes
        // if (currentSats != lastSatCount)
        // {
        //     Serial.printf("[GPS] Satellites: %ld (HDOP: %.2f)\n",
        //                   currentSats, gps.getHDOP());
        //                   lastSatCount = currentSats;
        // }

        // Periodic status update with NMEA data monitoring
        if (now - lastStatusPrint > statusUpdateInterval)
        {
            uint32_t nmeaThisPeriod = nmeaCharCount - lastNmeaCount;

            // Serial.printf("[GPS] Status: %s | Sats: %ld | HDOP: %.2f | Connected: %s | NMEA chars/5s: %ld\n",
            //               gps.getStatusString(),
            //               gps.getSatelliteCount(),
            //               gps.getHDOP(),
            //               gps.isConnected() ? "Yes" : "No",
            //               nmeaThisPeriod);

            // Comprehensive GPS status update
            GPSLocation loc = gps.getLocation();
            GPSTime time = gps.getTime();

            Serial.printf("[GPS] Sats: %ld | HDOP: %.2f (%s)",
                          gps.getSatelliteCount(), gps.getHDOP(), gps.getStatusString());

            // Add location and speed if we have a fix
            if (gps.hasFix())
            {
                Serial.printf(" | Loc: %.6f, %.6f | Speed: %.1f mph",
                              loc.latitude, loc.longitude, gps.getSpeedMph());
            }

            // Add time if valid
            if (time.valid)
            {
                Serial.printf(" | Time: %02d:%02d:%02d UTC",
                              time.hour, time.minute, time.second);
            }

            Serial.println(); // End line

            lastStatusPrint = now;
            lastNmeaCount = nmeaCharCount;
        }

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

    // Enable battery charging (disabled by default)
    Serial.print("Enabling battery charging... ");
    display.getAmoled().enableCharge();
    Serial.println("OK");

    // Configure low voltage cutoff for battery protection
    Serial.print("Configuring low voltage cutoff... ");
    // Set low battery shutdown threshold to 10% (hardware protection)
    display.getAmoled().setLowBatShutdownThreshold(10);
    uint8_t threshold = display.getAmoled().getLowBatShutdownThreshold();
    Serial.printf("OK (set to %d%%)\n", threshold);

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
