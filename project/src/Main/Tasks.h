#ifndef TASKS_H
#define TASKS_H

namespace Tasks
{
    void taskConfigureLocalPeripherials(void* RTOSparameter = nullptr);
    void taskInitializeConnection(void* RTOSparameter = nullptr);
    void taskSendTelemetry(void* RTOSparameter = nullptr);
    void taskDoMeasurements(void* RTOSparameter = nullptr);

    namespace Helpers
    {
        void taskHelperDelayForSerialConnection(void);
        void taskHelperStartSleep();
    }
}

#endif
