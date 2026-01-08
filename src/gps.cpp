#include "gps.h"

// GPS baud rates to try during auto-detection
const uint32_t GPS::gpsTestBauds[] = {115200, 38400, 9600};

GPS::GPS()
    : gpsSerial(2)
{
    // Constructor initializes member variables
}

bool GPS::begin()
{
    Serial.println("Initializing GPS...");

#ifdef FIRST_TIME_GPS_SETUP
    Serial.println("FIRST TIME SETUP MODE - Will configure GPS to 115200 baud");
    // For first time setup, try common baud rates and configure to 115200
    static const uint32_t firstTimeBauds[] = {38400, 9600, 115200};
    static const int numFirstTimeBauds = 3;

    bool gpsFound = false;
    for (int i = 0; i < numFirstTimeBauds && !gpsFound; i++)
    {
        uint32_t testBaud = firstTimeBauds[i];
        Serial.printf("Testing GPS at %d baud... ", testBaud);

        gpsSerial.begin(testBaud, SERIAL_8N1, RXPin, TXPin);
        delay(100);

        unsigned long startTime = millis();
        int validSentences = 0;

        while (millis() - startTime < 2000)
        {
            while (gpsSerial.available())
            {
                if (gps.encode(gpsSerial.read()))
                {
                    validSentences++;
                }
            }
            delay(10);
        }

        if (validSentences > 0)
        {
            Serial.println("GPS found!");
            gpsFound = true;

            // Configure to 115200 if not already
            if (testBaud != 115200)
            {
                Serial.print("Configuring GPS to 115200 baud permanently... ");
                configureBaudRate();
                currentGPSBaud = 115200;
                Serial.println("Done");
                Serial.println("IMPORTANT: Comment out '#define FIRST_TIME_GPS_SETUP' and recompile!");
            }
            else
            {
                currentGPSBaud = 115200;
                Serial.println("GPS already at 115200 baud");
            }
        }
        else
        {
            Serial.println("No response");
            gpsSerial.end();
        }
    }
#else
    // Normal operation - GPS should already be configured to 115200 baud
    Serial.printf("Connecting to GPS at %d baud... ", currentGPSBaud);

    gpsSerial.begin(currentGPSBaud, SERIAL_8N1, RXPin, TXPin);
    delay(100);

    unsigned long startTime = millis();
    int validSentences = 0;
    bool gpsFound = false;

    while (millis() - startTime < 3000)
    {
        while (gpsSerial.available())
        {
            if (gps.encode(gpsSerial.read()))
            {
                validSentences++;
            }
        }
        delay(10);
    }

    if (validSentences > 0)
    {
        Serial.println("GPS connected!");
        gpsFound = true;
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("If this is your first time, uncomment '#define FIRST_TIME_GPS_SETUP'");
    }
#endif

    if (!gpsFound)
    {
        gpsConfigured = false;
        Serial.println("ERROR: GPS not found! Check wiring.");
        return false;
    }
    else
    {
        gpsConfigured = true;
        Serial.printf("GPS ready at %d baud\n", currentGPSBaud);
        fixStartTime = millis();
        return true;
    }
}

void GPS::loop()
{
    if (!gpsConfigured)
        return;

    // Limit serial reads to prevent blocking UI - process max 50 chars per loop
    int bytesRead = 0;
    while (gpsSerial.available() && bytesRead < 50)
    {
        gps.encode(gpsSerial.read());
        bytesRead++;
    }

    // Debug GPS communication every 5 seconds
    if (millis() - lastGpsDebug > 5000)
    {
        displayInfo();
        lastGpsDebug = millis();
    }

    gpsHDOP = gps.hdop.hdop();
    gpsSats = gps.satellites.value();

    if (gps.location.isValid() && gps.location.age() < 2000)
    {
        speed = gps.speed.mph();
        if (speed < 0.8)
        {
            speed = 0; // remove fluctuation when stopped
        }

        if (gpsFirstFix == 0)
        {
            fixDuration = millis() - fixStartTime;
            Serial.print("GPS Fix acquired in ");
            Serial.print(fixDuration / 1000.0);
            Serial.println(" seconds.");
            gpsFirstFix = 1;
        }

        // Record Maximum Speed Value if Good position fix
        if (gpsHDOP < 4)
        {
            if (speed > speedMax && speed < 200)
            {
                speedMax = speed;
            }
            if (speed < 1)
            {
                maxSpeedReset = 1;
            }
            if (speed > 10 && maxSpeedReset == 1)
            {
                maxSpeedReset = 0;
                speedMax = speed;
            }
        }

        // Record 0 - 60mph time if Good position fix
        if (gpsHDOP < 4)
        {
            if (speed <= 1)
            {
                zeroToSixtyArm = 0;
            }

            if (speed > 1 && speed < 60 && zeroToSixtyArm == 0)
            {
                zeroToSixtyTimer = millis();
                zeroToSixtyArm = 1;
                // Serial.println("Armed");
            }

            if (speed > 10 && zeroToSixtyArm == 1)
            {
                zeroToSixtyTime = (millis() - zeroToSixtyTimer) / 1000.00;
            }

            if (speed >= 60 && zeroToSixtyArm == 1)
            {
                zeroToSixtyArm = 2;
                lastZeroToSixty = zeroToSixtyTime;
                Serial.print("0-60: ");
                Serial.print(zeroToSixtyTime);
                Serial.println("s");
            }

            if (zeroToSixtyTime > 60)
            {
                zeroToSixtyTime = lastZeroToSixty;
                zeroToSixtyArm = 2;
                Serial.println("Timeout");
            }
        }
    }
    else
    {
        if (gpsFirstFix == 1)
        {
            Serial.println("Waiting for GPS");
            gpsFirstFix = 0;
            fixStartTime = millis();
        }
    }
}

void GPS::displayInfo()
{
    Serial.print(F("   HDH: "));
    if (gps.location.isValid())
    {
        Serial.print(gps.hdop.hdop(), 2);
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F("  Sats: "));
    if (gps.satellites.isValid())
    {
        Serial.print(gps.satellites.value());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid())
    {
        Serial.print(gps.date.month());
        Serial.print(F("/"));
        Serial.print(gps.date.day());
        Serial.print(F("/"));
        Serial.print(gps.date.year());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (gps.time.isValid() && gps.date.isValid())
    {
        // Display UTC time (GPS satellites provide UTC time)
        int utcHour = gps.time.hour();
        bool dst = isUKDST(gps.date.year(), gps.date.month(), gps.date.day(), utcHour);

        if (utcHour < 10)
            Serial.print(F("0"));
        Serial.print(utcHour);
        Serial.print(F(":"));
        if (gps.time.minute() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(F("."));
        if (gps.time.centisecond() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.centisecond());
        Serial.print(F(" UTC ("));
        Serial.print(dst ? F("BST") : F("GMT"));
        Serial.print(F(" conversion handled in display)"));
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    // Serial.println();
    // Serial.printf("Sentences: %d, Failed: %d", gps.sentencesWithFix(), gps.failedChecksum());
    // Serial.println();
}

bool GPS::isUKDST(int year, int month, int day, int hour)
{
    if (month < 3 || month > 10)
        return false; // Jan, Feb, Nov, Dec = no DST
    if (month > 3 && month < 10)
        return true; // Apr–Sep = always DST

    // Calculate last Sunday of March or October
    int lastSunday = 31 - ((5 * year / 4 + (month == 3 ? 4 : 1)) % 7);

    if (month == 3)
        return (day > lastSunday || (day == lastSunday && hour >= 1)); // DST starts at 1 AM UTC
    else if (month == 10)
        return (day < lastSunday || (day == lastSunday && hour < 1)); // DST ends at 1 AM UTC

    return false;
}

void GPS::configureGNSS()
{
    Serial.println("Configuring GPS+GLONASS+Galileo...");

    // UBX-CFG-GNSS: Enable GPS, GLONASS, Galileo
    uint8_t gnssConfig[] = {
        0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00,                                     // Header
        0x00, 0x00, 0x20, 0x07, 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, // GPS enabled
        0x01, 0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0x01,                         // SBAS enabled
        0x02, 0x04, 0x08, 0x00, 0x01, 0x00, 0x01, 0x01,                         // Galileo enabled
        0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01,                         // BeiDou disabled
        0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01,                         // IMES disabled
        0x05, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01,                         // QZSS disabled
        0x06, 0x08, 0x0E, 0x00, 0x01, 0x00, 0x01, 0x01,                         // GLONASS enabled
        0x30, 0xAD                                                              // Checksum
    };

    gpsSerial.write(gnssConfig, sizeof(gnssConfig));
    delay(100);

    // UBX-CFG-RATE: Set 1Hz update rate
    uint8_t rateConfig[] = {
        0xB5, 0x62, 0x06, 0x08, 0x06, 0x00,
        0xE8, 0x03, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x39};
    gpsSerial.write(rateConfig, sizeof(rateConfig));
    delay(100);

    // UBX-CFG-CFG: Save configuration to flash
    uint8_t saveConfig[] = {
        0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // Calculate checksum
    uint8_t ck_a = 0, ck_b = 0;
    for (int i = 2; i < sizeof(saveConfig) - 2; i++)
    {
        ck_a += saveConfig[i];
        ck_b += ck_a;
    }
    saveConfig[sizeof(saveConfig) - 2] = ck_a;
    saveConfig[sizeof(saveConfig) - 1] = ck_b;

    gpsSerial.write(saveConfig, sizeof(saveConfig));
    delay(500);

    Serial.println("Multi-GNSS configuration complete");
}

void GPS::configureBaudRate()
{
    // UBX-CFG-PRT command to set UART1 to 115200 baud
    uint8_t baudConfig[] = {
        0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, // Header: UBX-CFG-PRT, Length: 20 bytes
        0x01,                               // Port ID: UART1
        0x00,                               // Reserved
        0x00, 0x00,                         // txReady (not used)
        0xD0, 0x08, 0x00, 0x00,             // Mode: 8-bit, no parity, 1 stop bit
        0x00, 0xC2, 0x01, 0x00,             // Baud rate: 115200 (0x0001C200)
        0x07, 0x00,                         // Input protocols: UBX + NMEA
        0x03, 0x00,                         // Output protocols: UBX + NMEA
        0x00, 0x00,                         // Flags
        0x00, 0x00,                         // Reserved
        0x00, 0x00                          // Checksum (will be calculated)
    };

    // Calculate correct checksum for UBX-CFG-PRT
    uint8_t ckA = 0, ckB = 0;
    for (int i = 2; i < sizeof(baudConfig) - 2; i++)
    {
        ckA += baudConfig[i];
        ckB += ckA;
    }
    baudConfig[sizeof(baudConfig) - 2] = ckA;
    baudConfig[sizeof(baudConfig) - 1] = ckB;

    gpsSerial.write(baudConfig, sizeof(baudConfig));
    delay(500); // Wait for command to be processed

    // Switch to 115200 baud rate
    gpsSerial.end();
    gpsSerial.begin(115200, SERIAL_8N1, RXPin, TXPin);
    delay(500); // Wait for serial to stabilize

    // UBX-CFG-CFG: Save current configuration to flash/EEPROM
    uint8_t saveConfig[] = {
        0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, // Header: UBX-CFG-CFG, Length: 13 bytes
        0x00, 0x00, 0x00, 0x00,             // Clear mask (none)
        0x1F, 0x1F, 0x00, 0x00,             // Save mask (ioPort, msgConf, infMsg, navConf, rxmConf)
        0x00, 0x00, 0x00, 0x00,             // Load mask (none)
        0x17,                               // Device mask: BBR + Flash + EEPROM
        0x31, 0xBF                          // Checksum (calculated)
    };

    // Calculate correct checksum for UBX-CFG-CFG
    uint8_t ckA2 = 0, ckB2 = 0;
    for (int i = 2; i < sizeof(saveConfig) - 2; i++)
    {
        ckA2 += saveConfig[i];
        ckB2 += ckA2;
    }
    saveConfig[sizeof(saveConfig) - 2] = ckA2;
    saveConfig[sizeof(saveConfig) - 1] = ckB2;

    gpsSerial.write(saveConfig, sizeof(saveConfig));
    delay(1000); // Wait for save operation to complete

    Serial.println("Baud rate saved to GPS flash memory");
}
