#include "Network.h"
// FIXME: Find out why is it that if I include this in the header file instead of her
// compilation fails!
#include <azure_ca.h>

static const char *SSID = CONFIG_WIFI_SSID;
static const char *PASSWORD = CONFIG_WIFI_PASSWORD;

static esp_mqtt_client_handle_t mqtt_client;
static char inbound_data[INBOUND_DATA_SIZE_BYTES];
static uint8_t telemetry_payload[100];
static char telemetry_topic[128];
static uint32_t telemetry_send_count = 0;

static az_iot_hub_client client;
static uint8_t sas_signature_buffer[256];
static char mqtt_client_id[128];
static char mqtt_username[128];
static char mqtt_password[200];

static const char *host = CONFIG_AZURE_FQDN;
static const char *device_id = CONFIG_AZURE_DEVICE_ID;
static const char *mqtt_broker_uri = "mqtts://" CONFIG_AZURE_FQDN;
static const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;

static AzIoTSasToken sasToken(
    &client,
    AZ_SPAN_FROM_STR(CONFIG_AZURE_DEVICE_KEY),
    AZ_SPAN_FROM_BUFFER(sas_signature_buffer),
    AZ_SPAN_FROM_BUFFER(mqtt_password));

namespace Setup
{

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

    void tryConnection()
    {
        Setup::WiFi_Connect();
        Setup::setupTime();
        IoTHub::initializeIoTHubClient();
        (void)MQTT::initializeMQTTClient();
    }
}

namespace IoTHub
{
    void initializeIoTHubClient()
    {

        az_iot_hub_client_options IoTHubClientOptions = az_iot_hub_client_options_default();
        IoTHubClientOptions.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

        // Initialize the IoT Hub client.
        az_result az_IoT_hub_result = az_iot_hub_client_init(
            &client,
            az_span_create((uint8_t *)host, strlen(host)),
            az_span_create((uint8_t *)device_id, strlen(device_id)),
            &IoTHubClientOptions);
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
            &client_id_length);
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
            NULL);
        if (az_result_failed(az_IoT_hub_result))
        {
            Logger.Error("Failed getting MQTT username.");
            return;
        }

        Logger.Info("Client ID: " + String(mqtt_client_id));
        Logger.Info("Username: " + String(mqtt_username));
    }

    // NOTE: This might not be good, because I do not know the internal working of az_span_copy (using a pointer might
    // change the behaviour of it, resulting in a badly manipulated payload).
    void getTelemetryPayload(az_span *payload)
    {
        az_span payload_holder = *payload;

        *payload = az_span_copy(*payload, AZ_SPAN_FROM_STR("{ \"msgCount\": "));
        (void)az_span_u32toa(*payload, telemetry_send_count++, payload);
        *payload = az_span_copy(*payload, AZ_SPAN_FROM_STR(" }"));
        *payload = az_span_copy_u8(*payload, NULL_TERMINATOR);

        *payload = az_span_slice(payload_holder, 0, az_span_size(payload_holder) - az_span_size(*payload));
    }

    void sendTelemetry()
    {
        az_span payload = AZ_SPAN_FROM_BUFFER(telemetry_payload);
        Logger.Info("Trying to send telemetry...");

        az_result result = az_iot_hub_client_telemetry_get_publish_topic(
            &client,
            NULL,
            telemetry_topic,
            sizeof(telemetry_topic),
            NULL);
        if (az_result_failed(result))
        {
            Logger.Error("Failed to get telemetry topic for c2d messages, with code: " + String(result));
            return;
        }

        IoTHub::getTelemetryPayload(&payload);

        result = esp_mqtt_client_publish(
            mqtt_client,
            telemetry_topic,
            (const char *)az_span_ptr(payload),
            az_span_size(payload),
            CONFIG_MQTT_CLIENT_QOS,
            CONFIG_MQTT_CLIENT_MESSAGE_RETAIN_POLICY);

        if (result == 0)
        {
            Logger.Error("Publishing of payload failed! No telemetry was sent  :( ");
        }
        else
        {
            Logger.Info("Message published successfully!  :) ");
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
            Logger.Info("MQTT event: MQTT_EVENT_ERROR");
            break;

        case MQTT_EVENT_CONNECTED:
            Logger.Info("MQTT event: MQTT_EVENT_CONNECTED");

            subscribe_message_id = esp_mqtt_client_subscribe(mqtt_client,
                                                             AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC,
                                                             CONFIG_MQTT_CLIENT_QOS);

            if (subscribe_message_id == -1)
            {
                Logger.Error("Could not subscribe to topic " + String(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC) + " with QoS level of " + String(CONFIG_MQTT_CLIENT_QOS));
            }
            else
            {
                Logger.Info("Subscribed to topic " + String(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC) + " with message ID " + String(subscribe_message_id) + " with QoS level of " + String(CONFIG_MQTT_CLIENT_QOS));
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
            for (int i = 0; i < (INBOUND_DATA_SIZE_BYTES_LAST_POS && i < (event->topic_len)); i++)
            {
                inbound_data[i] = event->topic[i];
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

    // TODO: This should rather return esp_err_t.
    int initializeMQTTClient()
    {
        int token_generation_result = sasToken.Generate(SAS_TOKEN_DURATION_IN_MINUTES);
        if (token_generation_result != SAS_TOKEN_GENERATION_OK)
        {
            Logger.Error("SAS token generation failed with code: " + String(token_generation_result));
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
            Logger.Error("Failed creating MQTT client.");
            return 2;
        }

        esp_err_t start_result = esp_mqtt_client_start(mqtt_client);

        if (start_result != ESP_OK)
        {
            Logger.Error("Could not start MQTT client with code: " + String(start_result));
            return 3;
        }
        else
        {
            Logger.Info("MQTT client started...");
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
