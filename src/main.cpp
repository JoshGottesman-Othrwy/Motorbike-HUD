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

    // Initialize other modules
    bool gpsOk = gpsModule.begin();
    bool accOk = acc.begin();

    // Button needs reference to the display's amoled object
    bool buttonOk = button.begin(display.getAmoled());

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
    // Add your network credentials here
    wifiManager.addNetwork("Livebox-7D40", "qHyiY3LATUsdc3SiJK");
    wifiManager.addNetwork("Home Wifi", "OrangeMonkeyEagle");
    // Add more networks as needed

    // Optional: Remove the comment below to enable WiFi startup
    wifiManager.begin();

    upTime = millis();
    Serial.println("System initialized successfully!");
}

void loop()
{
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
            false); // IMU not yet implemented

        lastWiFiUpdate = millis();
    }

    // Update display with GPS data
    if (gpsModule.isConfigured())
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

        // Update display refresh rate based on GPS fix status
        display.setFirstFix(gpsModule.isFirstFix());
    }

    // Handle display updates - always call loop() so button events can be processed
    display.loop();

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
