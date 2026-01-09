#pragma once
#include <Arduino.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

/**
 * GPS Status enumeration for qualitative accuracy reporting.
 * Based on HDOP (Horizontal Dilution of Precision) values.
 */
enum class GPSStatus
{
    NotConnected, // GPS module not responding
    NoFix,        // GPS connected but no satellite fix
    Poor,         // HDOP > 5.0 - Poor accuracy
    Fair,         // HDOP 2.0 - 5.0 - Fair accuracy
    Good,         // HDOP 1.0 - 2.0 - Good accuracy
    Excellent     // HDOP < 1.0 - Excellent accuracy
};

/**
 * GPS Time structure for returning time data.
 */
struct GPSTime
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    bool valid;
};

/**
 * GPS Location structure for returning position data.
 */
struct GPSLocation
{
    double latitude;
    double longitude;
    double altitude; // in meters
    bool valid;
};

/**
 * GPS module handler class.
 * Handles initialization, polling, and data retrieval from a GPS module.
 */
class GPS
{
private:
    // Hardware configuration
    static constexpr int RX_PIN = 44;
    static constexpr int TX_PIN = 43;
    static constexpr uint32_t GPS_BAUD = 9600;

    // Common GPS baud rates to try
    static constexpr uint32_t BAUD_RATES[] = {115200, 57600, 38400, 9600, 4800};
    static constexpr size_t NUM_BAUD_RATES = sizeof(BAUD_RATES) / sizeof(BAUD_RATES[0]);
    size_t currentBaudIndex = 0;
    uint32_t baudTestStartTime = 0;
    static constexpr uint32_t BAUD_TEST_DURATION = 5000; // Test each baud for 5 seconds

    // GPS objects
    TinyGPSPlus gps;
    HardwareSerial &gpsSerial;

    // State tracking
    bool initialized = false;
    bool moduleDetected = false;
    uint32_t lastDataTime = 0;
    static constexpr uint32_t DATA_TIMEOUT_MS = 3000; // Consider disconnected after 3s of no data

    // NMEA sentence debugging
    uint32_t lastDebugPrint = 0;
    static constexpr uint32_t DEBUG_INTERVAL_MS = 10000; // Print NMEA stats every 10s

    // Internal methods
    void processIncomingData();
    void configureConstellations();
    GPSStatus calculateStatus(float hdop);

public:
    GPS();

    /**
     * Initialize the GPS module.
     * Sets up serial communication and configures the GPS.
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * Process incoming GPS data.
     * Call this regularly in your main loop to keep GPS data updated.
     */
    void loop();

    /**
     * Get the current GPS status.
     * Returns qualitative accuracy based on HDOP value.
     * @return GPSStatus enum value
     */
    GPSStatus getStatus();

    /**
     * Get the GPS status as a human-readable string.
     * @return String representation of current status
     */
    const char *getStatusString();

    /**
     * Get the current time from GPS.
     * @return GPSTime structure with hour, minute, second, and validity flag
     */
    GPSTime getTime();

    /**
     * Get the current speed in mph.
     * @return Speed in miles per hour, or 0.0 if no valid fix
     */
    float getSpeedMph();

    /**
     * Get the current speed in km/h.
     * @return Speed in kilometers per hour, or 0.0 if no valid fix
     */
    float getSpeedKmh();

    /**
     * Get the current location.
     * @return GPSLocation structure with lat, lon, altitude, and validity flag
     */
    GPSLocation getLocation();

    /**
     * Get the number of satellites in view.
     * @return Number of satellites being tracked
     */
    uint32_t getSatelliteCount();

    /**
     * Get the raw HDOP value.
     * @return HDOP value, or 99.9 if not available
     */
    float getHDOP();

    /**
     * Check if GPS module is connected and responding.
     * @return true if module is responding with data
     */
    bool isConnected() const;

    /**
     * Check if GPS has a valid position fix.
     * @return true if location data is valid
     */
    bool hasFix();

    /**
     * Get the total number of NMEA characters processed.
     * Useful for debugging to confirm data is being received.
     * @return Total character count from TinyGPS++
     */
    uint32_t getCharsProcessed() const { return gps.charsProcessed(); }

    /**
     * Get reference to the underlying TinyGPS++ object.
     * For advanced usage if needed.
     */
    TinyGPSPlus &getRawGPS() { return gps; }
};
