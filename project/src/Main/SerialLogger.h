// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef SERIALLOGGER_H
#define SERIALLOGGER_H

#include <Arduino.h>
#include "Common.h"

#ifndef SERIAL_LOGGER_BAUD_RATE
#define SERIAL_LOGGER_BAUD_RATE DEFAULT_SERIAL_BAUD_RATE
#endif

class SerialLogger
{
public:
  SerialLogger();
  void Info(String message);
  void Error(String message);
};

extern SerialLogger Logger;

#endif // SERIALLOGGER_H
