#include "acc.h"

Accelerometer::Accelerometer() : compass(&Wire, QMC5883_ADDRESS) {
    // Constructor
}

bool Accelerometer::begin() {
    Serial.print("Initializing Compass... ");
    
    if (compass.begin()) {
        Serial.println("OK");
        compass.setRange(QMC5883_RANGE_2GA);
        compass.setMeasurementMode(QMC5883_CONTINOUS);
        compass.setDataRate(QMC5883_DATARATE_50HZ);
        compass.setSamples(QMC5883_SAMPLES_8);
        compass.setDeclinationAngle(declinationAngle);
        compassInitialized = true;
        return true;
    } else {
        Serial.println("Not found (check wiring)");
        compassInitialized = false;
        return false;
    }
}

void Accelerometer::loop() {
    if (!compassInitialized) return;
    
    // Update compass reading
    sVector_t mag = compass.readRaw();
    heading = mag.HeadingDegress;
}
