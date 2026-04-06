#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <atomic>

enum class WiFiOTAState
{
    Disabled,  // WiFi radio off
    Scanning,  // Attempting connection
    Connected, // WiFi up, OTA active
};

class WiFiOTA
{
private:
    static WiFiOTA *instance;

    WiFiOTAState state = WiFiOTAState::Disabled;
    std::atomic<bool> rescanRequested{false};
    std::atomic<bool> otaInProgress{false};

    uint32_t lastConnectionCheck = 0;
    static constexpr uint32_t CONNECTION_CHECK_INTERVAL = 10000; // 10s
    static constexpr uint32_t CONNECT_TIMEOUT_MS = 10000;        // 10s

    bool attemptConnection();
    void setupOTA();
    void disableWiFi();

public:
    WiFiOTA() = default;

    static WiFiOTA &getInstance();

    /**
     * Attempt WiFi connection on boot.
     * Blocks up to CONNECT_TIMEOUT_MS.
     * Returns true if connected.
     */
    bool begin();

    /**
     * Call from sensor task each iteration.
     * Handles OTA, connection monitoring, and rescan requests.
     */
    void handle();

    /**
     * Request a WiFi rescan from any core (thread-safe).
     */
    void requestRescan();

    WiFiOTAState getState() const { return state; }
    bool isConnected() const { return state == WiFiOTAState::Connected; }
    String getSSID() const;
    String getIP() const;
};
