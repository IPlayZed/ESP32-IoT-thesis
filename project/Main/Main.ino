// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include "src/Tasks/Tasks.hpp"

void setup()
{
        Tasks::taskConfigureLocalPeripherials();
        Tasks::taskInitializeConnection();
}

void loop()
{
        Tasks::taskDoMeasurements();
        Tasks::taskSendTelemetry();
        Tasks::Helpers::taskHelperStartSleep();
}