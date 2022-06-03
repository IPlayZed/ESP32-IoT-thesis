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

StaticJsonDocument<128> telemetry_msg;
static char serialized_telemetry_msg[128];

static AzIoTSasToken sasToken(
    &client,
    AZ_SPAN_FROM_STR(CONFIG_AZURE_DEVICE_KEY),
    AZ_SPAN_FROM_BUFFER(sas_signature_buffer),
    AZ_SPAN_FROM_BUFFER(mqtt_password));

namespace Setup
{

    void WiFi_Connect()
    {

#ifdef DEBUG_MODE
        Logger.Info("Trying to connect to " + String(CONFIG_WIFI_SSID));
#endif

        WiFi.mode(WIFI_STA);
        WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);

#ifdef DEBUG_MODE
            Serial.print(".");
#endif
        }

#ifdef DEBUG_MODE
        Serial.println();
        Logger.Info("Connected to " + String(CONFIG_WIFI_SSID) + " with IP of " + WiFi.localIP().toString());
#endif
    }

    void setupTime()
    {

#ifdef DEBUG_MODE
        Logger.Info("Initializing time via SNTP...");
#endif

        configTime(TIME_ZONE_GMT_OFFSET * TIME_S_TO_H_FACTOR, TIME_DAYLIGHT_SAVING_SECS, NTP_SERVERS_URL);
        time_t now = time(NULL);
        while (now < 1510592825)
        {
            delay(500);

#ifdef DEBUG_MODE
            Serial.print(".");
#endif

            now = time(nullptr);
        }

#ifdef DEBUG_MODE
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
#endif
    }

    void tryConnection()
    {
        Setup::WiFi_Connect();
        Setup::setupTime();
        IoTHub::initializeIoTHubClient();
    }
}

namespace IoTHub
{
    void initializeIoTHubClient()
    {

#ifdef DEBUG_MODE
        Logger.Info("Initializing IoT Hub client...");
#endif

        az_iot_hub_client_options IoTHubClientOptions = az_iot_hub_client_options_default();
        IoTHubClientOptions.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

        az_result az_IoT_hub_result = az_iot_hub_client_init(
            &client,
            az_span_create((uint8_t *)host, strlen(host)),
            az_span_create((uint8_t *)device_id, strlen(device_id)),
            &IoTHubClientOptions);
        if (az_result_failed(az_IoT_hub_result))
        {

#ifdef DEBUG_MODE
            Logger.Error("Failed to initialize Azure IoT Hub Client.");
#endif

            return;
        }

#ifdef DEBUG_MODE
        else
        {
            Logger.Info("Successfully initialized IoT Hub client!");
        }
#endif

        // Get the client ID length.

        size_t client_id_length;
        az_IoT_hub_result = az_iot_hub_client_get_client_id(
            &client,
            mqtt_client_id,
            sizeof(mqtt_client_id) - 1,
            &client_id_length);
        if (az_result_failed(az_IoT_hub_result))
        {

#ifdef DEBUG_MODE
            Logger.Error("Failed getting client MQTT ID!");
#endif

            return;
        }

#ifdef DEBUG_MODE
        else
        {
            Logger.Info("Got MQTT client ID: " + String(az_IoT_hub_result));
        }
#endif

        // Get the client username.
        az_IoT_hub_result = az_iot_hub_client_get_user_name(
            &client,
            mqtt_username,
            sizeofarray(mqtt_username),
            NULL);
        if (az_result_failed(az_IoT_hub_result))
        {

#ifdef DEBUG_MODE
            Logger.Error("Failed getting MQTT username!");
#endif

            return;
        }

#ifdef DEBUG_MODE
        else
        {
            Logger.Info("Got MQTT client username.");
        }

        Logger.Info("Client ID: " + String(mqtt_client_id));
        Logger.Info("Username: " + String(mqtt_username));
#endif
    }

    void getTelemetryPayload(az_span *payload)
    {
        telemetry_send_count++;
        az_span payload_holder = *payload;

        *payload = az_span_copy(*payload, AZ_SPAN_FROM_STR("{ \"msgCount\": "));
        (void)az_span_u32toa(*payload, telemetry_send_count, payload);
        *payload = az_span_copy(*payload, AZ_SPAN_FROM_STR(" }"));
        *payload = az_span_copy_u8(*payload, NULL_TERMINATOR);

        *payload = az_span_slice(payload_holder, 0, az_span_size(payload_holder) - az_span_size(*payload));
    }

    void sendTelemetry()
    {
        az_span payload = AZ_SPAN_FROM_BUFFER(telemetry_payload);

#ifdef DEBUG_MODE
        Logger.Info("Trying to send telemetry...");
#endif

        az_result result = az_iot_hub_client_telemetry_get_publish_topic(
            &client,
            NULL,
            telemetry_topic,
            sizeof(telemetry_topic),
            NULL);
        if (az_result_failed(result))
        {

#ifdef DEBUG_MODE
            Logger.Error("Failed to get telemetry topic for c2d messages, with code: " + String(result));
#endif
            return;
        }

        telemetry_msg["msgCount"] = telemetry_send_count;
        serializeJson(telemetry_msg, serialized_telemetry_msg);      
        
        Logger.Info("DEBUG: Serialized JSON to: \"" + String(serialized_telemetry_msg) + "\"");

        result = esp_mqtt_client_publish(
            mqtt_client,
            telemetry_topic,
            serialized_telemetry_msg,
            sizeofarray(serialized_telemetry_msg),
            CONFIG_MQTT_CLIENT_QOS,
            CONFIG_MQTT_CLIENT_MESSAGE_RETAIN_POLICY);

        telemetry_send_count++;
        

#ifdef DEBUG_MODE
        if (result == 0)
        {
            Logger.Error("Publishing of payload failed! No telemetry was NOT sent  :( ");
        }
        else
        {
            Logger.Info("Message published successfully!  :) ");
        }
#endif
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

#ifdef DEBUG_MODE
            Logger.Info("MQTT event: MQTT_EVENT_ERROR");
#endif
            break;

        case MQTT_EVENT_CONNECTED:

#ifdef DEBUG_MODE
            Logger.Info("MQTT event: MQTT_EVENT_CONNECTED");
#endif

            subscribe_message_id = esp_mqtt_client_subscribe(mqtt_client,
                                                             AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC,
                                                             CONFIG_MQTT_CLIENT_QOS);

            if (subscribe_message_id == -1)
            {

#ifdef DEBUG_MODE
                Logger.Error("Could not subscribe to topic " + String(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC) + " with QoS level of " + String(CONFIG_MQTT_CLIENT_QOS));
#endif
            }
            else
            {

#ifdef DEBUG_MODE
                Logger.Info("Subscribed to topic " + String(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC) + " with message ID " + String(subscribe_message_id) + " with QoS level of " + String(CONFIG_MQTT_CLIENT_QOS));
#endif
            }
            break;

        case MQTT_EVENT_DISCONNECTED:

#ifdef DEBUG_MODE
            Logger.Info("MQTT event: MQTT_EVENT_DISCONNECTED");
#endif

            break;

        case MQTT_EVENT_SUBSCRIBED:

#ifdef DEBUG_MODE
            Logger.Info("MQTT event: MQTT_EVENT_SUBSCRIBED");
#endif

            break;

        case MQTT_EVENT_UNSUBSCRIBED:

#ifdef DEBUG_MODE
            Logger.Info("MQTT event: MQTT_EVENT_UNSUBSCRIBED");
#endif

            break;

        case MQTT_EVENT_PUBLISHED:

#ifdef DEBUG_MODE
            Logger.Info("MQTT event: MQTT_EVENT_PUBLISHED");
#endif

            break;

        case MQTT_EVENT_DATA:

#ifdef DEBUG_MODE
            Logger.Info("MQTT event: MQTT_EVENT_DATA");
#endif

            // TODO: The data handling should be extracted to a function when it is more complicated.
            for (int i = 0; i < (INBOUND_DATA_SIZE_BYTES_LAST_POS && i < (event->topic_len)); i++)
            {
                inbound_data[i] = event->topic[i];
            }
            inbound_data[INBOUND_DATA_SIZE_BYTES_LAST_POS] = NULL_TERMINATOR;

#ifdef DEBUG_MODE
            Logger.Info("Got topic named: " + String(inbound_data));
#endif

            break;

        case MQTT_EVENT_BEFORE_CONNECT:

#ifdef DEBUG_MODE
            Logger.Info("MQTT event: MQTT_EVENT_BEFORE_CONNECT");
#endif

            break;

        default:

#ifdef DEBUG_MODE
            Logger.Error("MQTT event: UNKNOWN");
#endif

            break;
        }

        return ESP_OK;
    }

    // TODO: This should rather return esp_err_t.
    esp_err_t initializeMQTTClient()
    {
        int token_generation_result = sasToken.Generate(SAS_TOKEN_DURATION_IN_MINUTES);
        if (token_generation_result != SAS_TOKEN_GENERATION_OK)
        {

#ifdef DEBUG_MODE
            Logger.Error("SAS token generation failed with code: " + String(token_generation_result));
#endif

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

#ifdef DEBUG_MODE
            Logger.Error("Failed creating MQTT client.");
#endif

            return 2;
        }

        esp_err_t start_result = esp_mqtt_client_start(mqtt_client);

        if (start_result != ESP_OK)
        {

#ifdef DEBUG_MODE
            Logger.Error("Could not start MQTT client with code: " + String(start_result));
#endif

            return 3;
        }
        else
        {

#ifdef DEBUG_MODE
            Logger.Info("MQTT client started...");
#endif

            return 0;
        }
    };

    bool checkIfSasTokenInstanceIsExpired()
    {
        return sasToken.IsExpired();
    }

    void destroyMQTTClientInstance()
    {
        (void)esp_mqtt_client_destroy(mqtt_client);
    }
}
