// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include "Tasks.h"

void setup()
{
        Tasks::Helpers::taskHelperDelayForSerialConnection();
        Tasks::taskConfigureLocalPeripherials();
        Tasks::taskInitializeConnection();
}

void loop()
{
        Tasks::taskDoMeasurements();
        Tasks::taskSendTelemetry();
        Tasks::Helpers::taskHelperStartSleep();
}