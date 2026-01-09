#include "GPS.h"

// Define static constexpr array
constexpr uint32_t GPS::BAUD_RATES[];

GPS::GPS() : gpsSerial(Serial1)
{
    // Constructor - serial will be initialized in begin()
}

bool GPS::begin()
{
    Serial.println("GPS: Starting baud rate auto-detection...");

    // Start with first baud rate
    currentBaudIndex = 0;
    baudTestStartTime = millis();

    // Initialize with first baud rate
    gpsSerial.begin(BAUD_RATES[currentBaudIndex], SERIAL_8N1, RX_PIN, TX_PIN);
    Serial.printf("GPS: Testing baud rate %ld...\n", BAUD_RATES[currentBaudIndex]);

    delay(100);
    initialized = true;

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
    bool receivedData = false;
    uint32_t validSentencesBefore = gps.passedChecksum(); // Use total valid sentences, not just fix sentences

    while (gpsSerial.available() > 0)
    {
        char c = gpsSerial.read();
        receivedData = true; // Any data means module is connected

        // Check for end of sentence
        if (c == '\n')
        {
            // End of sentence - update timing but no debug output (handled in main.cpp)
        }

        if (gps.encode(c))
        {
            lastDataTime = millis();
        }
    }

    // Check if we got valid sentences at current baud rate
    uint32_t validSentencesAfter = gps.passedChecksum(); // Use total valid sentences, not just fix sentences
    if (validSentencesAfter > validSentencesBefore)
    {
        // Found valid sentences! Lock in this baud rate
        if (!moduleDetected)
        {
            moduleDetected = true;
            Serial.printf("[GPS] SUCCESS! Found correct baud rate: %ld (valid sentences: %ld)\n",
                          BAUD_RATES[currentBaudIndex], validSentencesAfter);
            // Configure for multiple constellations
            configureConstellations();
        }
    }
    else
    {
        // Check if we should try next baud rate (even if we're getting data, it might be corrupted)
        uint32_t now = millis();
        if (now - baudTestStartTime > BAUD_TEST_DURATION)
        {
            // Only stay on current baud if we're getting valid sentences
            if (validSentencesAfter > 0)
            {
                // We have valid sentences, keep this baud rate
                if (!moduleDetected)
                {
                    moduleDetected = true;
                    Serial.printf("[GPS] SUCCESS! Found correct baud rate: %ld (valid sentences: %ld)\n",
                                  BAUD_RATES[currentBaudIndex], validSentencesAfter);
                    // Configure for multiple constellations
                    configureConstellations();
                }
            }
            else
            {
                // No valid sentences after 5 seconds, try next baud rate
                currentBaudIndex++;
                if (currentBaudIndex < NUM_BAUD_RATES)
                {
                    Serial.printf("[GPS] Baud %ld failed, trying %ld...\n",
                                  BAUD_RATES[currentBaudIndex - 1], BAUD_RATES[currentBaudIndex]);
                    gpsSerial.end();
                    delay(100);
                    gpsSerial.begin(BAUD_RATES[currentBaudIndex], SERIAL_8N1, RX_PIN, TX_PIN);
                    baudTestStartTime = now;

                    // Clear TinyGPS++ state for fresh start
                    gps = TinyGPSPlus();
                }
                else
                {
                    // All baud rates failed, restart cycle
                    Serial.println("[GPS] All baud rates failed, restarting detection...");
                    currentBaudIndex = 0;
                    gpsSerial.end();
                    delay(100);
                    gpsSerial.begin(BAUD_RATES[currentBaudIndex], SERIAL_8N1, RX_PIN, TX_PIN);
                    baudTestStartTime = now;
                    gps = TinyGPSPlus();
                }
            }
        }
    }

    // Update connection status based on any received data
    if (receivedData)
    {
        lastDataTime = millis();
        // Only mark as detected if we're getting valid sentences or still testing baud rates
        if (!moduleDetected && validSentencesAfter > validSentencesBefore)
        {
            moduleDetected = true;
        }
    }

    // Check for data timeout - no data at all means disconnected
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

    // Then check if module is currently responding with NMEA data
    if (!moduleDetected)
    {
        return GPSStatus::NotConnected;
    }

    // Module is connected and sending data, now check fix quality
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
    // Return satellite count even if not fully valid (for tracking before fix)
    return gps.satellites.value();
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

void GPS::configureConstellations()
{
    Serial.println("[GPS] Configuring multi-constellation support...");

    // Send NMEA commands to enable multiple constellations
    // These work with most modern GPS modules
    delay(500); // Wait for module to stabilize

    // Enable GPS + GLONASS + Galileo + BeiDou
    // PMTK commands (common for many GPS modules)
    gpsSerial.println("$PMTK353,1,1,1,1,0*2A"); // GPS + GLONASS + Galileo + BeiDou + QZSS off
    delay(100);

    // Alternative: Try u-blox style commands
    gpsSerial.println("$PUBX,40,GLL,0,0,0,0,0,0*5C"); // Disable GLL (reduce NMEA traffic)
    delay(100);
    gpsSerial.println("$PUBX,40,VTG,0,0,0,0,0,0*5E"); // Disable VTG
    delay(100);
    gpsSerial.println("$PUBX,40,GSV,0,1,0,0,0,0*59"); // Enable GSV for satellite info
    delay(100);

    // Generic NMEA command to request satellite info
    gpsSerial.println("$PMTK314,0,1,0,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C");
    delay(100);

    // Set update rate to 5Hz for faster acquisition
    gpsSerial.println("$PMTK220,200*2C"); // 200ms = 5Hz
    delay(100);

    Serial.println("[GPS] Multi-constellation configuration sent");
    Serial.println("[GPS] Note: Module must support these constellations to see improvement");
}
