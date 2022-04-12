#include "Network.h"

static const char* SSID = CONFIG_WIFI_SSID;
static const char* PASSWORD = CONFIG_WIFI_PASSWORD;

static esp_mqtt_client_handle_t mqtt_client;
static char inbound_data[INBOUND_DATA_SIZE_BYTES];

static az_iot_hub_client client;
static uint8_t sas_signature_buffer[256];
static char mqtt_client_id[128];
static char mqtt_username[128];
static char mqtt_password[200];

static const char* host = CONFIG_AZURE_FQDN;
static const char* device_id = CONFIG_AZURE_DEVICE_ID;

void WiFi_Connect()
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

void setupTime() 
{
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

esp_err_t MQTTEventHandler(esp_mqtt_event_handle_t event)
{
    int subscribe_message_id = 0;
    switch (event -> event_id)
    {
        case MQTT_EVENT_ERROR:
            Logger.Info("MQTT event: MQTT_EVENT_ERROR");
            break;
        
        case MQTT_EVENT_CONNECTED:
            Logger.Info("MQTT event: MQTT_EVENT_CONNECTED");
                
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
            Logger.Info("MQTT event: MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            Logger.Info("MQTT event: MQTT_EVENT_SUBSCRIBED");
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            Logger.Info("MQTT event: MQTT_EVENT_UNSUBSCRIBED");
            break;

        case MQTT_EVENT_PUBLISHED:
            Logger.Info("MQTT event: MQTT_EVENT_PUBLISHED");
            break;

        case MQTT_EVENT_DATA:
            Logger.Info("MQTT event: MQTT_EVENT_DATA");
            for (int i = 0; i < (INBOUND_DATA_SIZE_BYTES_LAST_POS && i < (event -> topic_len)); i++)
            {
                inbound_data[i] = event -> topic[i];
            }
            inbound_data[INBOUND_DATA_SIZE_BYTES_LAST_POS] = NULL_TERMINATOR;

            // TODO: The data handling should be extracted to a function when it is more complicated.
            Logger.Info("Got topic named: " + String(inbound_data));
            
            break;

        case MQTT_EVENT_BEFORE_CONNECT:
            Logger.Info("MQTT event: MQTT_EVENT_BEFORE_CONNECT");
            break;

        default:
            Logger.Error("MQTT event: UNKNOWN");
            break;
                
    }

    return ESP_OK;
}

// TODO: Implement this.
int initializeMQTTClient()
{
    return 0;
};

void initializeIoTHubClient()
{

    az_iot_hub_client_options IoTHubClientOptions = az_iot_hub_client_options_default();
    IoTHubClientOptions.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

    // Initialize the IoT Hub client.
    az_result az_IoT_hub_result = az_iot_hub_client_init(
        &client,
        az_span_create((uint8_t*) host, strlen(host)),
        az_span_create((uint8_t*) device_id, strlen(device_id)),
        &IoTHubClientOptions
        ); 
    if (az_result_failed(az_IoT_hub_result))
    {
        Logger.Error("Failed to initialize Azure IoT Hub Client.");
        return;
    }

    // Get the client ID length.
    size_t client_id_length = 0;
    az_IoT_hub_result = az_iot_hub_client_get_client_id(
        &client,
        mqtt_client_id,
        sizeof(mqtt_client_id - 1),
        &client_id_length
    );
    if (az_result_failed(az_IoT_hub_result))
    {
        Logger.Error("Failed getting client MQTT ID.");
        return;
    }

    // Get the client username.
    az_IoT_hub_result = az_iot_hub_client_get_user_name(
        &client,
        mqtt_username,
        sizeofarray(mqtt_username),
        NULL
    );
    if (az_result_failed(az_IoT_hub_result))
    {
        Logger.Error("Failed getting MQTT username.");
        return;
    }

    Logger.Info("Client ID: " + String(mqtt_client_id));
    Logger.Info("Username: " + String(mqtt_username));
}

void tryConnection()
{
    WiFi_Connect();
    setupTime();
    initializeIoTHubClient();
    (void)initializeMQTTClient();
}