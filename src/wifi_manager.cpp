#include "wifi_manager.h"

// Global pointer for OTA callbacks
WiFiManager *g_wifiManager = nullptr;

WiFiManager::WiFiManager()
{
    g_wifiManager = this;
}

void WiFiManager::addNetwork(const char *ssid, const char *password)
{
    networks.push_back({ssid, password});
    Serial.printf("Added WiFi network: %s\n", ssid);
}

void WiFiManager::clearNetworks()
{
    networks.clear();
    Serial.println("Cleared all WiFi networks");
}

bool WiFiManager::begin()
{
    Serial.println("Starting WiFi Manager...");

    if (networks.empty())
    {
        Serial.println("No networks configured!");
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.onEvent([](WiFiEvent_t event)
                 {
        if (event == SYSTEM_EVENT_STA_GOT_IP) {
            Serial.print("WiFi connected! IP: ");
            Serial.println(WiFi.localIP());
        } else if (event == SYSTEM_EVENT_STA_DISCONNECTED) {
            Serial.println("WiFi disconnected");
        } });

    return connect();
}

bool WiFiManager::connect()
{
    if (isConnected)
    {
        return true;
    }

    unsigned long currentTime = millis();
    if (currentTime - lastConnectionAttempt < connectionRetryInterval)
    {
        return false;
    }

    lastConnectionAttempt = currentTime;

    Serial.println("Attempting to connect to WiFi networks...");

    for (size_t i = 0; i < networks.size(); i++)
    {
        const char *ssid = networks[i].ssid;
        const char *password = networks[i].password;

        Serial.printf("Trying: %s\n", ssid);

        WiFi.begin(ssid, password);

        // Wait up to 10 seconds for connection
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 10)
        {
            delay(300);
            Serial.print(".");
            attempts++;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.printf("Connected to: %s\n", ssid);
            isConnected = true;
            setupOTA();
            return true;
        }
    }

    Serial.println("Failed to connect to any network");
    return false;
}

bool WiFiManager::disconnect()
{
    if (!isConnected)
    {
        return false;
    }

    WiFi.disconnect();
    isConnected = false;
    Serial.println("WiFi disconnected");
    return true;
}

const char *WiFiManager::getSSID() const
{
    if (isConnected)
    {
        return WiFi.SSID().c_str();
    }
    return "Not Connected";
}

const char *WiFiManager::getLocalIP() const
{
    if (isConnected)
    {
        return WiFi.localIP().toString().c_str();
    }
    return "0.0.0.0";
}

void WiFiManager::setupOTA()
{
    Serial.println("Setting up OTA updates...");

    // Set hostname for OTA
    ArduinoOTA.setHostname("Motorbike-HUD");

    // Set OTA password (optional but recommended)
    ArduinoOTA.setPassword("Admin");

    ArduinoOTA.onStart([this]()
                       { handleOTAStart(); });

    ArduinoOTA.onEnd([this]()
                     { handleOTAEnd(); });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total)
                          { handleOTAProgress(progress, total); });

    ArduinoOTA.onError([this](ota_error_t error)
                       { handleOTAError(error); });

    ArduinoOTA.begin();
    Serial.println("OTA updates ready. You can now upload over-the-air!");
}

void WiFiManager::handleOTAStart()
{
    Serial.println("\nStarting OTA update...");
    otaInProgress = true;
    otaSuccess = false;
}

void WiFiManager::handleOTAEnd()
{
    Serial.println("\nOTA update completed!");
    otaInProgress = false;
    otaSuccess = true;
}

void WiFiManager::handleOTAProgress(unsigned int progress, unsigned int total)
{
    static unsigned long lastPrint = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastPrint > 1000)
    {
        int percent = (progress / (total / 100));
        Serial.printf("OTA Progress: %d%% (%u / %u bytes)\n", percent, progress, total);
        lastPrint = currentTime;
    }
}

void WiFiManager::handleOTAError(ota_error_t error)
{
    Serial.print("OTA Error: ");
    switch (error)
    {
    case OTA_AUTH_ERROR:
        Serial.println("Auth Failed");
        break;
    case OTA_BEGIN_ERROR:
        Serial.println("Begin Failed");
        break;
    case OTA_CONNECT_ERROR:
        Serial.println("Connect Failed");
        break;
    case OTA_RECEIVE_ERROR:
        Serial.println("Receive Failed");
        break;
    case OTA_END_ERROR:
        Serial.println("End Failed");
        break;
    default:
        Serial.println("Unknown Error");
        break;
    }
    otaInProgress = false;
}

void WiFiManager::loop()
{
    // Attempt to reconnect if disconnected
    if (!isConnected && millis() - lastConnectionAttempt > connectionRetryInterval)
    {
        connect();
    }

    // Handle OTA updates if connected
    if (isConnected)
    {
        ArduinoOTA.handle();
    }
}