#pragma once
#include <Arduino.h>
#include <DFRobot_QMC5883.h>
#include <Wire.h>

class Accelerometer {
private:
    DFRobot_QMC5883 compass;
    bool compassInitialized = false;
    float heading = 0.0;
    float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / PI); // UK declination
    
public:
    Accelerometer();
    bool begin();
    void loop();
    
    // Getters
    float getHeading() const { return heading; }
    bool isInitialized() const { return compassInitialized; }
};
