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
    statusMessage = "Starting WiFi";
    Serial.println(statusMessage);

    if (networks.empty())
    {
        statusMessage = "No networks configured!";
        Serial.println(statusMessage);
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.onEvent([](WiFiEvent_t event)
                 {
        if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
            Serial.print("WiFi connected! IP: ");
            Serial.println(WiFi.localIP());
        } else if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
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

    // Start non-blocking connection attempt
    if (connectionState == IDLE)
    {
        lastConnectionAttempt = currentTime;
        currentNetworkIndex = 0;
        connectionState = CONNECTING;
        statusMessage = "Attempting to connect";
        Serial.println("Attempting to connect to WiFi networks...");
    }

    return false; // Will return true when actually connected via handleConnection()
}

void WiFiManager::handleConnection()
{
    unsigned long currentTime = millis();

    switch (connectionState)
    {
    case IDLE:
        break;

    case CONNECTING:
        if (currentNetworkIndex < networks.size())
        {
            const char *ssid = networks[currentNetworkIndex].ssid;
            const char *password = networks[currentNetworkIndex].password;

            statusMessage = String("Trying: ") + ssid;
            Serial.printf("Trying: %s\n", ssid);

            WiFi.begin(ssid, password);
            connectionStartTime = currentTime;
            connectionState = WAITING_FOR_RESULT;
        }
        else
        {
            // Tried all networks, failed
            statusMessage = "Failed to connect";
            Serial.println("Failed to connect to any network");
            connectionState = IDLE;
        }
        break;

    case WAITING_FOR_RESULT:
        if (WiFi.status() == WL_CONNECTED)
        {
            statusMessage = "Connected";
            Serial.printf("Connected to: %s\n", networks[currentNetworkIndex].ssid);
            isConnected = true;
            connectionState = IDLE;
            setupOTA();
        }
        else if (currentTime - connectionStartTime > connectionTimeout)
        {
            // Timeout, try next network
            Serial.println(" timeout");
            currentNetworkIndex++;
            connectionState = CONNECTING;
        }
        break;
    }
}

bool WiFiManager::disconnect()
{
    if (!isConnected)
    {
        return false;
    }

    WiFi.disconnect();
    isConnected = false;
    statusMessage = "Disconnected";
    Serial.println("WiFi disconnected");
    return true;
}

const char *WiFiManager::getSSID() const
{
    static String ssidStr;
    if (isConnected)
    {
        ssidStr = WiFi.SSID();
        return ssidStr.c_str();
    }
    return "Not Connected";
}

const char *WiFiManager::getLocalIP() const
{
    static String ipStr;
    if (isConnected)
    {
        ipStr = WiFi.localIP().toString();
        return ipStr.c_str();
    }
    return "0.0.0.0";
}

const char *WiFiManager::getStatusMessage() const
{
    return statusMessage.c_str();
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
    // Handle non-blocking connection state machine
    handleConnection();

    // Attempt to reconnect if disconnected
    if (!isConnected && connectionState == IDLE)
    {
        connect();
    }

    // Handle OTA updates if connected
    if (isConnected)
    {
        ArduinoOTA.handle();
    }
}