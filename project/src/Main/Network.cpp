// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include <azure_ca.h> // This must be included here, because the lib doesn't define include guards for this header file...
#include <ArduinoJson.h>
#include <time.h>
#include <WiFi.h>
#include <mqtt_client.h>

#include <az_core.h>
#include <az_iot.h>

#include "CommonConfig.h"
#include "Network.h"
#include "NetworkConfig.h"

#include "AzIoTSasToken.h"
#include "SecretsConfig.h"

#include "SerialLogger.h"
#include "NetworkConfig.h"

#define MAGIC_TIMESTAMP (uint32_t)1510592825

#define AZURE_SDK_CLIENT_USER_AGENT "c/" AZ_SDK_VERSION_STRING "(ard;esp32)"
#define TIMEINFO_FORMAT_STRING "%A, %B %d %Y %H:%M:%S"

namespace Network
{
    // Thesese variables are used by both the Network::MQTT:: and Network::IoTHub namespaces, so they are declared here.

    static char mqtt_password[200];
    static const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;
    static const char *mqtt_broker_uri = "mqtts://" CONFIG_AZURE_FQDN;
    static esp_mqtt_client_handle_t mqtt_client;
    static char mqtt_client_id[128];
    static char mqtt_username[128];
    static char telemetry_topic[128];

    static az_iot_hub_client client;
    static uint8_t sas_signature_buffer[256];
    static AzIoTSasToken sasToken(
            &Network::client,
            AZ_SPAN_FROM_STR(CONFIG_AZURE_DEVICE_KEY),
            AZ_SPAN_FROM_BUFFER(Network::sas_signature_buffer),
            AZ_SPAN_FROM_BUFFER(Network::mqtt_password));

    static char serializedTelemetryData[CONFIG_TELEMETRY_DATA_MAXIMUM_SIZE];
    
    namespace _WiFi
    {
        void _connect(void)
        {
            LogInfo("Trying to connect to " + String(CONFIG_WIFI_SSID));
            WiFi.mode(WIFI_STA);
            WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
            uint8_t wait_tick = 0;
            while (WiFi.status() != WL_CONNECTED)
            {
                wait_tick++;
                if (wait_tick > CONFIG_WIFI_WAIT_TICKS)
                {
                    ESP.restart();
                }
                
                delay(CONFIG_WIFI_WAIT_MSEC);
                SerialPrint('.');
            }

            SerialPrintln();
            LogInfo("Connected to " + String(CONFIG_WIFI_SSID) + " with IP of " + WiFi.localIP().toString());
        }
    }

    namespace SNTP
    {
        void _setup(void)
        {
            LogInfo("Initializing time via SNTP...");
            configTime(TIME_ZONE_GMT_OFFSET * TIME_S_TO_H_FACTOR, TIME_DAYLIGHT_SAVING_SECS, NTP_SERVERS_URL);
            time_t now = time(NULL);
            uint8_t wait_tick = 0;
            while (now < MAGIC_TIMESTAMP)
            {
                wait_tick++;
                if (wait_tick > CONFIG_SNTP_WAIT_TICKS)
                {
                    ESP.restart();
                }
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
                SerialPrintlnTimeinfo(&timeinfo, TIMEINFO_FORMAT_STRING);
            }
        }
    }

    namespace MQTT
    {
        esp_err_t _MQTTEventHandler(esp_mqtt_event_handle_t event)
        {
            int subscribe_message_id = 0;
            switch (event->event_id)
            {
            case MQTT_EVENT_ERROR:
                LogInfo("MQTT event: MQTT_EVENT_ERROR");
                break;

            case MQTT_EVENT_CONNECTED:
                LogInfo("MQTT event: MQTT_EVENT_CONNECTED");
                subscribe_message_id = esp_mqtt_client_subscribe(Network::mqtt_client,
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

        // FIXME: Find out why does this cause "Core  1 panic'ed (StoreProhibited)"
        void _configureMQTTConfiguration(esp_mqtt_client_config_t* mqtt_configuration)
        {
            memset(mqtt_configuration, 0, sizeof(mqtt_configuration));
            mqtt_configuration -> uri = Network::mqtt_broker_uri;
            mqtt_configuration -> port = Network::mqtt_port;
            mqtt_configuration -> client_id = Network::mqtt_client_id;
            mqtt_configuration -> username = Network::mqtt_username;
            mqtt_configuration -> password = (const char *)az_span_ptr(Network::sasToken.Get());
            mqtt_configuration -> keepalive = 30;
            mqtt_configuration -> disable_clean_session = 0;
            mqtt_configuration -> disable_auto_reconnect = false;
            mqtt_configuration -> event_handle = MQTT::_MQTTEventHandler;
            mqtt_configuration -> user_context = NULL;
            mqtt_configuration -> cert_pem = (const char *)ca_pem;
        }

        void _initializeMQTTClient(void)
        {
            int token_generation_result = Network::sasToken.Generate(CONFIG_SAS_TOKEN_DURATION_IN_MINUTES);
            if (token_generation_result != SAS_TOKEN_GENERATION_OK)
            {
                return LogError("SAS token generation failed with code: " + String(token_generation_result));
            }

            esp_mqtt_client_config_t mqtt_configuration;
            
            memset(&mqtt_configuration, 0, sizeof(mqtt_configuration));
            mqtt_configuration.uri = Network::mqtt_broker_uri;
            mqtt_configuration.port = Network::mqtt_port;
            mqtt_configuration.client_id = Network::mqtt_client_id;
            mqtt_configuration.username = Network::mqtt_username;
            mqtt_configuration.password = (const char *)az_span_ptr(Network::sasToken.Get());
            mqtt_configuration.keepalive = 30;
            mqtt_configuration.disable_clean_session = 0;
            mqtt_configuration.disable_auto_reconnect = false;
            mqtt_configuration.event_handle = MQTT::_MQTTEventHandler;
            mqtt_configuration.user_context = NULL;
            mqtt_configuration.cert_pem = (const char *)ca_pem;
            
            //configureMQTTConfiguration(&mqtt_configuration); // FIXME: Find out why does this cause "Core  1 panic'ed (StoreProhibited)"

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

        bool _checkIfSasTokenInstanceIsExpired(void)
        {
            return Network::sasToken.IsExpired();
        }

        void _destroyMQTTClientInstance(void)
        {
            (void)esp_mqtt_client_destroy(Network::mqtt_client);
        }
    }

    namespace IoTHub
    {
        void _initializeIoTHubClient(void)
        {
            LogInfo("Initializing IoT Hub client...");

            az_iot_hub_client_options IoTHubClientOptions = az_iot_hub_client_options_default();
            IoTHubClientOptions.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

            az_result az_IoT_hub_result = az_iot_hub_client_init(
                &client,
                az_span_create((uint8_t *)CONFIG_AZURE_FQDN, strlen(CONFIG_AZURE_FQDN)),
                az_span_create((uint8_t *)CONFIG_AZURE_DEVICE_ID, strlen(CONFIG_AZURE_DEVICE_ID)),
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
                &Network::client,
                Network::mqtt_client_id,
                sizeof(Network::mqtt_client_id) - 1,
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
                &Network::client,
                Network::mqtt_username,
                sizeofarray(Network::mqtt_username),
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
            LogInfo("Client ID: " + String(Network::mqtt_client_id) + " Username: " + String(Network::mqtt_username));
        }

        void _sendTelemetry()
        {
            LogInfo("Trying to send telemetry...");

            az_result result = az_iot_hub_client_telemetry_get_publish_topic(
                &Network::client,
                NULL,
                Network::telemetry_topic,
                sizeof(Network::telemetry_topic),
                NULL);
            if (az_result_failed(result))
            {
                LogError("Failed to get telemetry topic for c2d messages, with code: " + String(result));
                return;
            }

            if (serializedTelemetryData == nullptr)
            {
                return LogError("The serialized data was null.");
            }

            result = esp_mqtt_client_publish(
                    Network::mqtt_client,
                    Network::telemetry_topic,
                    Network::serializedTelemetryData,
                    strlen(Network::serializedTelemetryData),
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

    namespace Telemetry
    {
        void processTelemetryData(Network::Telemetry::telemetryData_t* telemetryData)
        {
            if (telemetryData == nullptr)
            {
                return LogError("Telemetry data was null.");
            }

            StaticJsonDocument<CONFIG_TELEMETRY_DATA_MAXIMUM_SIZE> telemetry_message;
            telemetry_message[CONFIG_TELEMETRY_DATA_TEMPERATURE_ID] = telemetryData->temperature;
            telemetry_message[CONFIG_TELEMETRY_DATA_HUMIDITY_ID] = telemetryData->humidity;
            telemetry_message[CONFIG_TELEMETRY_DATA_CO_ID] = telemetryData->CO;
            serializeJson(telemetry_message, serializedTelemetryData);

            LogInfo("Serialized message: \"" + String(serializedTelemetryData) + "\"");
        }

        void sendTelemetry()
        {
            if (WiFi.getMode() == WIFI_OFF)
            {
                LogInfo("WiFi was turned off, turning on as station again for telemetry.");
                WiFi.mode(WIFI_STA);
            }
            
            if (WiFi.status() != WL_CONNECTED)
            {
                LogInfo("WiFi is not connected to an AP, trying to reconnect.");
                Network::_WiFi::_connect();
                Network::MQTT::_initializeMQTTClient();
            }
            else if (Network::MQTT::_checkIfSasTokenInstanceIsExpired())
            {
                LogInfo("SAS token expired; reconnecting with a new one.");
                Network::MQTT::_destroyMQTTClientInstance();
                Network::MQTT::_initializeMQTTClient();
            }

            LogInfo("Task now trying to send telemetry....");
            Network::IoTHub::_sendTelemetry();

            LogInfo("Telemetry sending done disconnecting WiFi, and entering deep sleep in 2 seconds for " + String(CONFIG_TIME_TO_SLEEP_IN_S) + "...\n");
            Network::MQTT::_destroyMQTTClientInstance();
            WiFi.disconnect(true, true);
        }
    }

    void turnOffWiFi(void)
    {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }
    void setupNetworking(bool turnOffWifiAfterSetup)
    {
        Network::_WiFi::_connect();
        Network::SNTP::_setup();
        Network::IoTHub::_initializeIoTHubClient();
        if (turnOffWifiAfterSetup) Network::turnOffWiFi();
    }
}
