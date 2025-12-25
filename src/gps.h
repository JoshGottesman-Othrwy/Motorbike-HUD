#pragma once
#include <Arduino.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

class GPS
{
private:
    // Hardware configuration
    static const int RXPin = 44;
    static const int TXPin = 43;
    static const uint32_t gpsTestBauds[];
    static const int numTestBauds = 3;
    uint32_t currentGPSBaud = 115200;

    // GPS objects
    TinyGPSPlus gps;
    HardwareSerial gpsSerial;

    // GPS state variables
    bool gpsConfigured = false;
    bool hotStartEnabled = false;
    bool gpsFirstFix = false;
    unsigned long timeToFix = 0;
    unsigned long fixDuration = 0;
    unsigned long fixStartTime = 0;
    unsigned long lastGpsDebug = 0;

    // GPS data
    float speed = 0.00;
    float speedMax = 0.00;
    float gpsHDOP = 100;
    int gpsSats = 0;
    bool maxSpeedReset = false;

    // 0-60 timing
    long zeroToSixtyTimer = 0;
    long lastZeroToSixty = 0;
    float zeroToSixtyTime = 0;
    int zeroToSixtyArm = 0;

    // Private helper methods
    void configureGNSS();
    void configureBaudRate();
    void displayInfo();
    bool isUKDST(int year, int month, int day, int hour);

public:
    GPS();
    bool begin();
    void loop();

    // Getters
    float getSpeed() const { return speed; }
    float getSpeedMax() const { return speedMax; }
    float getGpsHDOP() const { return gpsHDOP; }
    int getGpsSats() const { return gpsSats; }
    float getZeroToSixtyTime() const { return zeroToSixtyTime; }
    bool isFirstFix() const { return gpsFirstFix; }
    bool isConfigured() const { return gpsConfigured; }
    bool isLocationValid() const { return gps.location.isValid() && gps.location.age() < 2000; }
    TinyGPSPlus &getGPS() { return gps; }
    bool getUKDST(int year, int month, int day, int hour) { return isUKDST(year, month, day, hour); }

    // Setters
    void addSpeedOffset(int offset) { speed += offset; }
};
