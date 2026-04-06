#include "WiFiOTA.h"

WiFiOTA *WiFiOTA::instance = nullptr;

WiFiOTA &WiFiOTA::getInstance()
{
    if (instance == nullptr)
    {
        instance = new WiFiOTA();
    }
    return *instance;
}

bool WiFiOTA::begin()
{
    instance = this;

    // Check if credentials are configured
    const char *ssid = WIFI_SSID;
    if (strlen(ssid) == 0)
    {
        Serial.println("[WiFi] No SSID configured - WiFi disabled");
        state = WiFiOTAState::Disabled;
        return false;
    }

    Serial.printf("[WiFi] Attempting connection to '%s'...\n", ssid);
    state = WiFiOTAState::Scanning;

    if (attemptConnection())
    {
        setupOTA();
        state = WiFiOTAState::Connected;
        Serial.printf("[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    }
    else
    {
        Serial.println("[WiFi] Connection failed - disabling WiFi");
        disableWiFi();
        return false;
    }
}

void WiFiOTA::handle()
{
    // Handle rescan requests (from UI button, thread-safe)
    if (rescanRequested.exchange(false))
    {
        Serial.println("[WiFi] Rescan requested");
        state = WiFiOTAState::Scanning;

        if (attemptConnection())
        {
            setupOTA();
            state = WiFiOTAState::Connected;
            Serial.printf("[WiFi] Reconnected! IP: %s\n", WiFi.localIP().toString().c_str());
        }
        else
        {
            Serial.println("[WiFi] Rescan failed - disabling WiFi");
            disableWiFi();
        }
        return;
    }

    if (state != WiFiOTAState::Connected)
    {
        return;
    }

    // Handle OTA
    ArduinoOTA.handle();

    // Periodic connection health check (skip during active OTA transfer)
    uint32_t now = millis();
    if (!otaInProgress.load() && (now - lastConnectionCheck >= CONNECTION_CHECK_INTERVAL))
    {
        lastConnectionCheck = now;

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("[WiFi] Connection lost - disabling WiFi");
            disableWiFi();
        }
    }
}

void WiFiOTA::requestRescan()
{
    rescanRequested.store(true);
}

String WiFiOTA::getSSID() const
{
    if (state == WiFiOTAState::Connected)
    {
        return WiFi.SSID();
    }
    return "---";
}

String WiFiOTA::getIP() const
{
    if (state == WiFiOTAState::Connected)
    {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

bool WiFiOTA::attemptConnection()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < CONNECT_TIMEOUT_MS)
    {
        delay(100);
    }

    return WiFi.status() == WL_CONNECTED;
}

void WiFiOTA::setupOTA()
{
    ArduinoOTA.setHostname("Motorbike-HUD");

    ArduinoOTA.onStart([this]()
                       {
        otaInProgress.store(true);
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.printf("[OTA] Start updating %s\n", type.c_str()); });

    ArduinoOTA.onEnd([this]()
                     {
        otaInProgress.store(false);
        Serial.println("\n[OTA] Update complete"); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100))); });

    ArduinoOTA.onError([this](ota_error_t error)
                       {
        otaInProgress.store(false);
        const char *msg = "Unknown";
        switch (error) {
            case OTA_AUTH_ERROR:    msg = "Auth Failed"; break;
            case OTA_BEGIN_ERROR:   msg = "Begin Failed"; break;
            case OTA_CONNECT_ERROR: msg = "Connect Failed"; break;
            case OTA_RECEIVE_ERROR: msg = "Receive Failed"; break;
            case OTA_END_ERROR:     msg = "End Failed"; break;
        }
        Serial.printf("[OTA] Error: %s\n", msg); });

    ArduinoOTA.begin();
    Serial.println("[OTA] Ready");
}

void WiFiOTA::disableWiFi()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    state = WiFiOTAState::Disabled;
}
