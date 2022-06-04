#include "Network.h"
#include <azure_ca.h> // This must be included here, because the lib doesn't define include guards for this header file...
#include <ArduinoJson.h>

static esp_mqtt_client_handle_t mqtt_client;
static char inbound_data[INBOUND_DATA_SIZE_BYTES];
static uint8_t telemetry_payload[256];
static char telemetry_topic[128];
uint32_t telemetry_send_count = 0;

static az_iot_hub_client client;
static uint8_t sas_signature_buffer[256];
static char mqtt_client_id[128];
static char mqtt_username[128];
static char mqtt_password[200];

static const char *host = CONFIG_AZURE_FQDN;
static const char *device_id = CONFIG_AZURE_DEVICE_ID;
static const char *mqtt_broker_uri = "mqtts://" CONFIG_AZURE_FQDN;
static const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;


static char serialized_telemetry_msg[128];

static AzIoTSasToken sasToken(
    &client,
    AZ_SPAN_FROM_STR(CONFIG_AZURE_DEVICE_KEY),
    AZ_SPAN_FROM_BUFFER(sas_signature_buffer),
    AZ_SPAN_FROM_BUFFER(mqtt_password));

namespace Setup
{

    void WiFi_Connect(void)
    {
        LogInfo("Trying to connect to " + String(CONFIG_WIFI_SSID));
        WiFi.mode(WIFI_STA);
        WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
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
            delay(500);
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
            LogInfo("Got local times: ");
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
            &client,
            mqtt_client_id,
            sizeof(mqtt_client_id) - 1,
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
            &client,
            mqtt_username,
            sizeofarray(mqtt_username),
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
        LogInfo("Client ID: " + String(mqtt_client_id) + " Username: " + String(mqtt_username));
    }

    void sendTelemetry(void)
    {
        az_span payload = AZ_SPAN_FROM_BUFFER(telemetry_payload);

        LogInfo("Trying to send telemetry...");

        az_result result = az_iot_hub_client_telemetry_get_publish_topic(
            &client,
            NULL,
            telemetry_topic,
            sizeof(telemetry_topic),
            NULL);
        if (az_result_failed(result))
        {
            LogError("Failed to get telemetry topic for c2d messages, with code: " + String(result));
            return;
        }

        StaticJsonDocument<128> telemetry_msg;
        telemetry_msg["msgCount"] = telemetry_send_count;
        serializeJson(telemetry_msg, serialized_telemetry_msg);      
        
        Logger.Info("DEBUG: Serialized JSON to: " + String(serialized_telemetry_msg));

        result = esp_mqtt_client_publish(
            mqtt_client,
            telemetry_topic,
            serialized_telemetry_msg,
            sizeofarray(serialized_telemetry_msg),
            CONFIG_MQTT_CLIENT_QOS,
            CONFIG_MQTT_CLIENT_MESSAGE_RETAIN_POLICY);

        telemetry_send_count++;
        
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
            subscribe_message_id = esp_mqtt_client_subscribe(mqtt_client,
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
            // TODO: The data handling should be extracted to a function when it is more complicated. (Maybe, but we do not really care about this data in my implementation (?))
            for (int i = 0; i < (INBOUND_DATA_SIZE_BYTES_LAST_POS && i < (event->topic_len)); i++)
            {
                inbound_data[i] = event->topic[i];
            }
            inbound_data[INBOUND_DATA_SIZE_BYTES_LAST_POS] = NULL_TERMINATOR;
            LogInfo("Got topic/data named: " + String(inbound_data));
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

    // TODO: Better error return types.
    esp_err_t initializeMQTTClient(void)
    {
        int token_generation_result = sasToken.Generate(SAS_TOKEN_DURATION_IN_MINUTES);
        if (token_generation_result != SAS_TOKEN_GENERATION_OK)
        {
            LogError("SAS token generation failed with code: " + String(token_generation_result));
            return 1;
        }

        esp_mqtt_client_config_t mqtt_configuration;
        memset(&mqtt_configuration, 0, sizeof(mqtt_configuration));
        memset(&mqtt_configuration, 0, sizeof(mqtt_configuration));
        mqtt_configuration.uri = mqtt_broker_uri;
        mqtt_configuration.port = mqtt_port;
        mqtt_configuration.client_id = mqtt_client_id;
        mqtt_configuration.username = mqtt_username;
        mqtt_configuration.password = (const char *)az_span_ptr(sasToken.Get());
        mqtt_configuration.keepalive = 30;
        mqtt_configuration.disable_clean_session = 0;
        mqtt_configuration.disable_auto_reconnect = false;
        mqtt_configuration.event_handle = MQTT::MQTTEventHandler;
        mqtt_configuration.user_context = NULL;
        mqtt_configuration.cert_pem = (const char *)ca_pem;

        mqtt_client = esp_mqtt_client_init(&mqtt_configuration);

        if (mqtt_client == NULL)
        {
            LogError("Failed creating MQTT client.");
            return 2;
        }

        esp_err_t start_result = esp_mqtt_client_start(mqtt_client);

        if (start_result != ESP_OK)
        {
            LogError("Could not start MQTT client with code: " + String(start_result));
            return 3;
        }
        else
        {
            LogInfo("MQTT client started...");
            return 0;
        }
    };

    bool checkIfSasTokenInstanceIsExpired(void)
    {
        return sasToken.IsExpired();
    }

    void destroyMQTTClientInstance(void)
    {
        (void)esp_mqtt_client_destroy(mqtt_client);
    }
}
