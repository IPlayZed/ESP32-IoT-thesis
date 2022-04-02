/**
 * @file Network.C
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-02 
 */

#include "Network.h"

static const char* SSID = CONFIG_WIFI_SSID;
static const char* PASSWORD = CONFIG_WIFI_PASSWORD;

static esp_mqtt_client_handle_t mqtt_client;

static void WiFi_Connect()
{
    Logger.Info("Trying to connect to " + String(SSID));

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    
    Serial.println();
    Logger.Info("Connected to " + String(SSID) + " with IP of " + WiFi.localIP().toString());
}

// For more information on the workings see https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/ 
static void setupTime() {
    Logger.Info("Initialize time via SNTP");
    configTime(TIME_ZONE_GMT_OFFSET * TIME_S_TO_H_FACTOR, TIME_DAYLIGHT_SAVING_SECS, NTP_SERVERS_URL);
      time_t now = time(NULL);
    while (now < 1510592825)
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("");
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Logger.Error("Failed to get local time!");
    }
    else
    {
        Logger.Info("Got local time: ");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    }
}

static esp_err_t MQTTEventHandler(esp_mqtt_event_handle_t event)
{
    switch (event -> event_id)
    {
        int subscribe_message_id = 0;

        case MQTT_EVENT_ERROR:
            Logger.Info("MQTT event ID: MQTT_EVENT_ERROR");
            break;
        
        case MQTT_EVENT_CONNECTED:
            Logger.Info("MQTT event ID: MQTT_EVENT_CONNECTED");
                
            subscribe_message_id = esp_mqtt_client_subscribe(mqtt_client,
            AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, DEFAULT_QOS);

            if (subscribe_message_id == -1)
            {
                Logger.Error("Could not subscribe to topic " 
                + String(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC) 
                + " with QoS level of " + String(DEFAULT_QOS));
            }
            else
            {
                Logger.Info("Subscribed to topic "
                +String(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC)
                + " with message ID " + String(subscribe_message_id)
                + " with QoS level of " + String(DEFAULT_QOS));
            }
            break;
        
        case MQTT_EVENT_DISCONNECTED:
            Logger.Info("MQTT event ID: MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            Logger.Info("MQTT event MQTT_EVENT_SUBSCRIBED");
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            Logger.Info("MQTT event MQTT_EVENT_UNSUBSCRIBED");
            break;

        case MQTT_EVENT_PUBLISHED:
            Logger.Info("MQTT event MQTT_EVENT_PUBLISHED");
            break;

        case MQTT_EVENT_DATA:
            Logger.Info("MQTT event MQTT_EVENT_DATA");
            // TODO: Implement data handling.

            break;

        case MQTT_EVENT_BEFORE_CONNECT:
            Logger.Info("MQTT event MQTT_EVENT_BEFORE_CONNECT");
            break;

        default:
            Logger.Error("MQTT event UNKNOWN");
            break;
            
    }
}
