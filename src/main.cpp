#include <Arduino.h>
#include "gps.h"
#include "acc.h"
#include "button.h"
#include "display.h"
#include "wifi_manager.h"

// FIRST TIME SETUP: Uncomment the line below if this is your first time running the code
// This will configure your GPS module to use 115200 baud permanently
// #define FIRST_TIME_GPS_SETUP

// Create class instances
GPS gpsModule;
Accelerometer acc;
Button button;
Display display;
WiFiManager wifiManager;

// Global variables
int myNumber = 0;
unsigned long upTime = 0;

void setup(void)
{
    delay(50);
    Serial.begin(115200);
    Serial.println("=======MOTO DISPLAY=======");

    // Initialize display first as other modules depend on it
    bool displayOk = display.begin();
    if (!displayOk)
    {
        Serial.println("Display initialization failed!");
        while (1)
        {
            delay(1000);
        }
    }

    // Show immediate feedback that display is working
    Serial.println("Display ready - showing startup status");
    display.setGPSStarting(); // Show 'Starting' status in yellow
    lv_task_handler();        // Process LVGL tasks to actually show the status

    // Initialize other modules
    Serial.print("Initializing GPS... ");
    bool gpsOk = gpsModule.begin();
    Serial.println(gpsOk ? "OK" : "FAILED");

    Serial.print("Initializing Accelerometer... ");
    bool accOk = acc.begin();
    Serial.println(accOk ? "OK" : "FAILED");

    // Button needs reference to the display's amoled object
    Serial.print("Initializing Buttons... ");
    bool buttonOk = button.begin(display.getAmoled());
    Serial.println(buttonOk ? "OK" : "FAILED");

    if (!gpsOk)
    {
        Serial.println("GPS initialization failed!");
    }

    if (!accOk)
    {
        Serial.println("Accelerometer initialization failed!");
    }

    if (!buttonOk)
    {
        Serial.println("Button initialization failed!");
    }

    // Initialize WiFi and OTA Updates
    Serial.println("Setting up WiFi networks...");
    // Add your network credentials here
    // wifiManager.addNetwork("Livebox-7D40", "qHyiY3LATUsdc3SiJK");
    wifiManager.addNetwork("Home Wifi", "OrangeMonkeyEagle");
    // wifiManager.addNetwork("Other Way - Guest", "Eng1neer1ng Th4t M4tters");
    // Add more networks as needed

    // Optional: Remove the comment below to enable WiFi startup
    Serial.print("Starting WiFi... ");
    bool wifiOk = wifiManager.begin();
    Serial.println(wifiOk ? "OK" : "FAILED");
    lv_task_handler(); // Update display after WiFi init

    upTime = millis();
    Serial.println("System initialized successfully!");
}

void loop()
{
    // Call LVGL task handler first for responsive UI
    lv_task_handler();

    // Call each module's loop function
    gpsModule.loop();
    acc.loop();
    button.loop();
    wifiManager.loop(); // Handle WiFi and OTA updates

    // Update WiFi info on display
    static unsigned long lastWiFiUpdate = 0;
    if (millis() - lastWiFiUpdate > 1000)
    {
        display.updateWiFiInfo(
            wifiManager.isWiFiConnected(),
            wifiManager.getSSID(),
            wifiManager.getLocalIP(),
            wifiManager.getStatusMessage());

        // Update module status
        display.updateModuleStatus(
            gpsModule.isConfigured(),
            false,                // IMU not yet implemented
            acc.isInitialized()); // Magnetometer status

        lastWiFiUpdate = millis();
    }

    // Update display with GPS data
    if (gpsModule.isConfigured())
    {
        // Check if GPS location is valid (has fix)
        if (gpsModule.isLocationValid())
        {
            // Add speed offset if needed
            gpsModule.addSpeedOffset(myNumber);

            // Update display with current GPS data
            display.updateGPSData(
                gpsModule.getSpeed(),
                gpsModule.getSpeedMax(),
                gpsModule.getGpsHDOP(),
                gpsModule.getGpsSats(),
                gpsModule.getZeroToSixtyTime());
        }
        else
        {
            // No GPS fix - show dash for speed and no fix status
            display.setSpeedDisplayNoFix();
            display.updateGPSData(0.0, 0.0, 100.0, 0, 0.0); // Show no fix state
        }

        // Update time display
        if (gpsModule.getGPS().time.isValid() && gpsModule.getGPS().date.isValid())
        {
            int utcHour = gpsModule.getGPS().time.hour();
            int localHour = utcHour;
            bool dst = gpsModule.getUKDST(gpsModule.getGPS().date.year(), gpsModule.getGPS().date.month(),
                                          gpsModule.getGPS().date.day(), utcHour);
            localHour += dst ? 2 : 1;
            if (localHour >= 24)
                localHour -= 24;
            if (localHour < 0)
                localHour += 24;
            display.updateTimeDisplay(true, localHour, gpsModule.getGPS().time.minute());
        }
        else
        {
            display.updateTimeDisplay(false, 0, 0);
        }

        // Note: Display refresh rate is now fixed regardless of GPS status
    }
    else
    {
        // GPS not configured - show no GPS state
        display.setSpeedDisplayNoFix();
        static unsigned long lastNoGPSUpdate = 0;
        static unsigned long lastDebugUpdate = 0;
        
        // Update GPS data every second when no GPS
        if (millis() - lastNoGPSUpdate > 1000)
        {
            display.updateGPSData(0.0, 0.0, 100.0, 0, 0.0); // Show "Sats. 0" and "No Fix"
            display.updateTimeDisplay(false, 0, 0);
            lastNoGPSUpdate = millis();
        }
        
        // Keep debug display flashing at normal rate even without GPS
        if (millis() - lastDebugUpdate > 16) // Same as normal updateDisplayInterval
        {
            // Simple debug toggle without full GPS data update
            static bool debugState = false;
            debugState = !debugState;
            // Note: This is a workaround - ideally we'd have a separate debug update method
            lastDebugUpdate = millis();
        }
    }

    // Handle button presses (example functionality)
    if (button.wasPressed())
    {
        Serial.println("Button state changed!");
        // Additional button functionality can be added here
    }

    // Optional: Print compass heading periodically
    static unsigned long lastCompassPrint = 0;
    if (millis() - lastCompassPrint > 5000 && acc.isInitialized())
    {
        Serial.print(F("Heading: "));
        Serial.print(acc.getHeading(), 1);
        Serial.println(F("°"));
        lastCompassPrint = millis();
    }
}
