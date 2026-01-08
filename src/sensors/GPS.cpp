#include "GPS.h"

GPS::GPS() : gpsSerial(Serial1)
{
    // Constructor - serial will be initialized in begin()
}

bool GPS::begin()
{
    Serial.println("GPS: Initializing...");

    // Initialize GPS serial port
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RX_PIN, TX_PIN);

    // Wait a moment for serial to stabilize
    delay(100);

    // Check if we receive any data from GPS module
    uint32_t startTime = millis();
    while (millis() - startTime < 2000)
    {
        if (gpsSerial.available())
        {
            moduleDetected = true;
            lastDataTime = millis();
            break;
        }
        delay(10);
    }

    if (moduleDetected)
    {
        Serial.println("GPS: Module detected");
        initialized = true;
    }
    else
    {
        Serial.println("GPS: Module not detected - check wiring");
        initialized = false;
    }

    return initialized;
}

void GPS::loop()
{
    if (!initialized)
    {
        return;
    }

    processIncomingData();
}

void GPS::processIncomingData()
{
    while (gpsSerial.available() > 0)
    {
        char c = gpsSerial.read();
        if (gps.encode(c))
        {
            lastDataTime = millis();
            moduleDetected = true;
        }
    }

    // Check for data timeout
    if (millis() - lastDataTime > DATA_TIMEOUT_MS)
    {
        moduleDetected = false;
    }
}

GPSStatus GPS::calculateStatus(float hdop)
{
    if (!moduleDetected)
    {
        return GPSStatus::NotConnected;
    }

    if (!hasFix())
    {
        return GPSStatus::NoFix;
    }

    // HDOP-based accuracy classification
    if (hdop < 1.0f)
    {
        return GPSStatus::Excellent;
    }
    else if (hdop < 2.0f)
    {
        return GPSStatus::Good;
    }
    else if (hdop <= 5.0f)
    {
        return GPSStatus::Fair;
    }
    else
    {
        return GPSStatus::Poor;
    }
}

GPSStatus GPS::getStatus()
{
    // First check if GPS module was never detected during initialization
    if (!initialized)
    {
        return GPSStatus::NotConnected;
    }

    // Then check if module is currently responding
    if (!moduleDetected)
    {
        return GPSStatus::NotConnected;
    }

    // Module is connected, now check fix quality
    return calculateStatus(getHDOP());
}

const char *GPS::getStatusString()
{
    switch (getStatus())
    {
    case GPSStatus::NotConnected:
        return "NC";
    case GPSStatus::NoFix:
        return "No Fix";
    case GPSStatus::Poor:
        return "Poor";
    case GPSStatus::Fair:
        return "Fair";
    case GPSStatus::Good:
        return "Good";
    case GPSStatus::Excellent:
        return "Excellent";
    default:
        return "Unknown";
    }
}

GPSTime GPS::getTime()
{
    GPSTime time;

    if (gps.time.isValid())
    {
        time.hour = gps.time.hour();
        time.minute = gps.time.minute();
        time.second = gps.time.second();
        time.valid = true;
    }
    else
    {
        time.hour = 0;
        time.minute = 0;
        time.second = 0;
        time.valid = false;
    }

    return time;
}

float GPS::getSpeedMph()
{
    if (gps.speed.isValid())
    {
        return static_cast<float>(gps.speed.mph());
    }
    return 0.0f;
}

float GPS::getSpeedKmh()
{
    if (gps.speed.isValid())
    {
        return static_cast<float>(gps.speed.kmph());
    }
    return 0.0f;
}

GPSLocation GPS::getLocation()
{
    GPSLocation loc;

    if (gps.location.isValid())
    {
        loc.latitude = gps.location.lat();
        loc.longitude = gps.location.lng();
        loc.altitude = gps.altitude.isValid() ? gps.altitude.meters() : 0.0;
        loc.valid = true;
    }
    else
    {
        loc.latitude = 0.0;
        loc.longitude = 0.0;
        loc.altitude = 0.0;
        loc.valid = false;
    }

    return loc;
}

uint32_t GPS::getSatelliteCount()
{
    if (gps.satellites.isValid())
    {
        return gps.satellites.value();
    }
    return 0;
}

float GPS::getHDOP()
{
    if (gps.hdop.isValid())
    {
        return static_cast<float>(gps.hdop.hdop());
    }
    return 99.9f; // Return high value if HDOP not available
}

bool GPS::isConnected() const
{
    return moduleDetected;
}

bool GPS::hasFix()
{
    // Consider we have a fix if location is valid and not too old
    return gps.location.isValid() && gps.location.age() < 2000;
}
