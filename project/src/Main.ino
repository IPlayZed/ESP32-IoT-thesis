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
#include "libraries/Common.h"
#include "libraries/Network.h"

static const BaseType_t PRO_CPU = PRO_CPU_NUM;
static const BaseType_t APP_CPU = APP_CPU_NUM;

void setup()
{
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_IN_S * uS_TO_S_FACTOR);
    // FIXME: Main.ino:19: undefined reference to `tryConnection()`
    tryConnection();
}

void loop()
{

}