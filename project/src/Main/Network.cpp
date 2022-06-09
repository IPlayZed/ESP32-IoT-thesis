// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include <azure_ca.h> // This must be included here, because the lib doesn't define include guards for this header file...
#include <ArduinoJson.h>

#include "Network.h"
#include "NetworkConfig.h"

#define MAGIC_TIMESTAMP (uint32_t)1510592825

static const int serialized_telemetry_message[128];

namespace Setup
{

    void WiFi_Connect(void)
    {
        LogInfo("Trying to connect to " + String(CONFIG_WIFI_SSID));
        WiFi.mode(WIFI_STA);
        WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);

        while (WiFi.status() != WL_CONNECTED)
        {
            // TODO: Make this configurable.
            delay(CONFIG_WIFI_WAIT_MSEC);
            SerialPrint('.');
        }

        SerialPrintln();
        LogInfo("Connected to " + String(CONFIG_WIFI_SSID) + " with IP of " + WiFi.localIP().toString());
    }

    void setupTime(void)
    {
        LogInfo("Initializing time via SNTP...");
        configTime(TIME_ZONE_GMT_OFFSET * TIME_S_TO_H_FACTOR, TIME_DAYLIGHT_SAVING_SECS, NTP_SERVERS_URL);
        time_t now = time(NULL);
        while (now < MAGIC_TIMESTAMP)
        {
            // TODO: Make this configurable.
            delay(CONFIG_TIME_WAIT_MSEC);
            SerialPrint('.');
            now = time(nullptr);
        }

        struct tm timeinfo;
        SerialPrintln();
        
        if (!getLocalTime(&timeinfo))
        {
            LogError("Failed to get local time!");
        }
        else
        {
            LogInfo("Got local time: ");
            // TODO: Better preprocessing conditional.
            #ifdef DEBUG_MODE
            Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
            #endif
        }
    }

    void tryConnection(void)
    {
        Setup::WiFi_Connect();
        Setup::setupTime();
        IoTHub::initializeIoTHubClient();
    }
}

namespace IoTHub
{
    static az_iot_hub_client client;
    static uint8_t sas_signature_buffer[256];
    static const char *host = CONFIG_AZURE_FQDN;
    static const char *device_id = CONFIG_AZURE_DEVICE_ID;

    static AzIoTSasToken sasToken(
        &client,
        AZ_SPAN_FROM_STR(CONFIG_AZURE_DEVICE_KEY),
        AZ_SPAN_FROM_BUFFER(IoTHub::sas_signature_buffer),
        AZ_SPAN_FROM_BUFFER(MQTT::mqtt_password));

    void initializeIoTHubClient(void)
    {
        LogInfo("Initializing IoT Hub client...");

        az_iot_hub_client_options IoTHubClientOptions = az_iot_hub_client_options_default();
        IoTHubClientOptions.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

        az_result az_IoT_hub_result = az_iot_hub_client_init(
            &client,
            az_span_create((uint8_t *)host, strlen(host)),
            az_span_create((uint8_t *)device_id, strlen(device_id)),
            &IoTHubClientOptions);
        if (az_result_failed(az_IoT_hub_result))
        {
            LogError("Failed to initialize Azure IoT Hub Client.");
            return;
        }
        else
        {
            LogInfo("Successfully initialized IoT Hub client!");
        }

        size_t client_id_length;
        az_IoT_hub_result = az_iot_hub_client_get_client_id(
            &IoTHub::client,
            MQTT::mqtt_client_id,
            sizeof(MQTT::mqtt_client_id) - 1,
            &client_id_length);
        if (az_result_failed(az_IoT_hub_result))
        {
            LogError("Failed getting client MQTT ID!");
            return;
        }
        else
        {
            LogInfo("Got MQTT client ID: " + String(az_IoT_hub_result));
        }

        az_IoT_hub_result = az_iot_hub_client_get_user_name(
            &IoTHub::client,
            MQTT::mqtt_username,
            sizeofarray(MQTT::mqtt_username),
            NULL);
        if (az_result_failed(az_IoT_hub_result))
        {
            LogError("Failed getting MQTT username!");
            return;
        }
        else
        {
            LogInfo("Got MQTT client username.");
        }
        LogInfo("Client ID: " + String(MQTT::mqtt_client_id) + " Username: " + String(MQTT::mqtt_username));
    }

    void sendTelemetry(void)
    {
        LogInfo("Trying to send telemetry...");

        az_result result = az_iot_hub_client_telemetry_get_publish_topic(
            &IoTHub::client,
            NULL,
            MQTT::telemetry_topic,
            sizeof(MQTT::telemetry_topic),
            NULL);
        if (az_result_failed(result))
        {
            LogError("Failed to get telemetry topic for c2d messages, with code: " + String(result));
            return;
        }

        StaticJsonDocument<128> telemetry_msg;
        telemetry_msg["msgCount"] = 1;  
        String serialized_telemetry_message;
        serializeJson(telemetry_msg, serialized_telemetry_message);
        LogInfo("Serialized msg: " + serialized_telemetry_message);
        result = esp_mqtt_client_publish(
                 MQTT::mqtt_client,
                 MQTT::telemetry_topic,
                 serialized_telemetry_message.c_str(),
                 serialized_telemetry_message.length(),
                 CONFIG_MQTT_CLIENT_QOS,
                 CONFIG_MQTT_CLIENT_MESSAGE_RETAIN_POLICY);
        
        if (result == 0)
        {
            LogError("Publishing of payload failed! No telemetry was NOT sent  :( ");
        }
        else
        {
            LogInfo("Message published successfully!  :) ");
        }  
    }
}
namespace MQTT
{
    static const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;
    static const char *mqtt_broker_uri = "mqtts://" CONFIG_AZURE_FQDN;

    static esp_mqtt_client_handle_t mqtt_client;
    static char mqtt_client_id[128];
    static char mqtt_username[128];
    static char mqtt_password[200];
    static char telemetry_topic[128];

    esp_err_t MQTTEventHandler(esp_mqtt_event_handle_t event)
    {
        int subscribe_message_id = 0;
        switch (event->event_id)
        {
        case MQTT_EVENT_ERROR:
            LogInfo("MQTT event: MQTT_EVENT_ERROR");
            break;

        case MQTT_EVENT_CONNECTED:
            LogInfo("MQTT event: MQTT_EVENT_CONNECTED");
            subscribe_message_id = esp_mqtt_client_subscribe(MQTT::mqtt_client,
                                                             AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC,
                                                             CONFIG_MQTT_CLIENT_QOS);

            if (subscribe_message_id == -1)
            {
                LogError("Could not subscribe to topic " + String(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC) 
                + " with QoS level of " + String(CONFIG_MQTT_CLIENT_QOS));
            }
            else
            {
                LogInfo("Subscribed to topic " 
                + String(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC) + " with message ID " 
                + String(subscribe_message_id) + " with QoS level of " + String(CONFIG_MQTT_CLIENT_QOS));
            }
            break;

        case MQTT_EVENT_DISCONNECTED:
            LogInfo("MQTT event: MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            LogInfo("MQTT event: MQTT_EVENT_SUBSCRIBED");
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            LogInfo("MQTT event: MQTT_EVENT_UNSUBSCRIBED");
            break;

        case MQTT_EVENT_PUBLISHED:
            LogInfo("MQTT event: MQTT_EVENT_PUBLISHED");
            break;

        case MQTT_EVENT_DATA:
            LogInfo("MQTT event: MQTT_EVENT_DATA");
            break;

        case MQTT_EVENT_BEFORE_CONNECT:
            LogInfo("MQTT event: MQTT_EVENT_BEFORE_CONNECT");
            break;

        default:
            LogError("MQTT event: UNKNOWN");
            break;
        }

        return ESP_OK;
    }

    void configureMQTTConfiguration(esp_mqtt_client_config_t* mqtt_configuration)
    {
        memset(&mqtt_configuration, 0, sizeof(mqtt_configuration));
        mqtt_configuration -> uri = MQTT::mqtt_broker_uri;
        mqtt_configuration -> port = MQTT::mqtt_port;
        mqtt_configuration -> client_id = MQTT::mqtt_client_id;
        mqtt_configuration -> username = MQTT::mqtt_username;
        mqtt_configuration -> password = (const char *)az_span_ptr(IoTHub::sasToken.Get());
        mqtt_configuration -> keepalive = 30;
        mqtt_configuration -> disable_clean_session = 0;
        mqtt_configuration -> disable_auto_reconnect = false;
        mqtt_configuration -> event_handle = MQTT::MQTTEventHandler;
        mqtt_configuration -> user_context = NULL;
        mqtt_configuration -> cert_pem = (const char *)ca_pem;
    }

    void initializeMQTTClient(void)
    {
        int token_generation_result = IoTHub::sasToken.Generate(CONFIG_SAS_TOKEN_DURATION_IN_MINUTES);
        if (token_generation_result != SAS_TOKEN_GENERATION_OK)
        {
            return LogError("SAS token generation failed with code: " + String(token_generation_result));
        }

        esp_mqtt_client_config_t mqtt_configuration;
        
        /*
        memset(&mqtt_configuration, 0, sizeof(mqtt_configuration));
        mqtt_configuration.uri = MQTT::mqtt_broker_uri;
        mqtt_configuration.port = MQTT::mqtt_port;
        mqtt_configuration.client_id = MQTT::mqtt_client_id;
        mqtt_configuration.username = MQTT::mqtt_username;
        mqtt_configuration.password = (const char *)az_span_ptr(IoTHub::sasToken.Get());
        mqtt_configuration.keepalive = 30;
        mqtt_configuration.disable_clean_session = 0;
        mqtt_configuration.disable_auto_reconnect = false;
        mqtt_configuration.event_handle = MQTT::MQTTEventHandler;
        mqtt_configuration.user_context = NULL;
        mqtt_configuration.cert_pem = (const char *)ca_pem;*/

        configureMQTTConfiguration(&mqtt_configuration); // TODO: Check if this works.

        mqtt_client = esp_mqtt_client_init(&mqtt_configuration);

        if (mqtt_client == NULL)
        {
            return LogError("Failed creating MQTT client.");;
        }

        esp_err_t start_result = esp_mqtt_client_start(mqtt_client);

        if (start_result != ESP_OK)
        {
            return LogError("Could not start MQTT client with code: " + String(start_result));
        }
        else
        {
            LogInfo("MQTT client started...");
            return;
        }
    };

    bool checkIfSasTokenInstanceIsExpired(void)
    {
        return IoTHub::sasToken.IsExpired();
    }

    void destroyMQTTClientInstance(void)
    {
        (void)esp_mqtt_client_destroy(MQTT::mqtt_client);
    }
}
