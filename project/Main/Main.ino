// Licensed under: GNU GENERAL PUBLIC LICENSE Version 2

#include "src/tasks/Tasks.hpp"

void setup()
{
        Tasks::configureLocalPeripherials();
        Tasks::initializeConnection();
}

void loop()
{
        Tasks::doMeasurements();
        Tasks::sendTelemetry();
        Tasks::Helpers::startSleep();
}