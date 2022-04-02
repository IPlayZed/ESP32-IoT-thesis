/**
 * @file Main.ino
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <time.h>

#include <mqtt_client.h>

#include "libraries/Common.h"

static const BaseType_t PRO_CPU = PRO_CPU_NUM;
static const BaseType_t APP_CPU = APP_CPU_NUM;




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
            break;
        
        subscribe_message_id = esp_mqtt_client_subscribe

        default:
            break;
    }
}

void setup()
{
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
}

void loop()
{

}