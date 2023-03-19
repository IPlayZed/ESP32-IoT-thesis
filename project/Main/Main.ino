// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include "src/Tasks/Tasks.hpp"

// TODO: In general, we should examine if header variables are supposed to be static or they should be extern's of the source globals.

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