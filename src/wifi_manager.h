#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <vector>

struct NetworkCredentials
{
    const char *ssid;
    const char *password;
};

class WiFiManager
{
private:
    std::vector<NetworkCredentials> networks;
    bool isConnected = false;
    unsigned long lastConnectionAttempt = 0;
    const unsigned long connectionRetryInterval = 60000; // 60 seconds
    String statusMessage = "Not started";

    // Non-blocking connection state
    enum ConnectionState
    {
        IDLE,
        CONNECTING,
        WAITING_FOR_RESULT
    } connectionState = IDLE;
    size_t currentNetworkIndex = 0;
    unsigned long connectionStartTime = 0;
    const unsigned long connectionTimeout = 10000; // 10 seconds per network

    // OTA Update state
    bool otaInProgress = false;
    bool otaSuccess = false;

    // Private methods
    void setupOTA();
    void handleConnection();
    void handleOTAStart();
    void handleOTAEnd();
    void handleOTAProgress(unsigned int progress, unsigned int total);
    void handleOTAError(ota_error_t error);

public:
    WiFiManager();

    // Network management
    void addNetwork(const char *ssid, const char *password);
    void clearNetworks();
    bool begin();
    void loop();
    bool connect();
    bool disconnect();

    // Status getters
    bool isWiFiConnected() const { return isConnected; }
    const char *getSSID() const;
    const char *getLocalIP() const;
    const char *getStatusMessage() const;

    // OTA Update status
    bool isOTARunning() const { return otaInProgress; }
    bool wasOTASuccessful() const { return otaSuccess; }
};