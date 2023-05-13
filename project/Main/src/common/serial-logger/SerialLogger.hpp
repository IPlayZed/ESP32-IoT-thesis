// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT
// Modified by project author.

#ifndef SERIALLOGGER_HPP
#define SERIALLOGGER_HPP

#include <Arduino.h>
#include "../CommonConfig.hpp"

#ifndef SERIAL_LOGGER_BAUD_RATE
#define SERIAL_LOGGER_BAUD_RATE CONFIG_SERIAL_BAUD_RATE
#endif

class SerialLogger
{
public:
  SerialLogger();
  void Info(String message);
  void Warning(String message);
  void Error(String message);
};

extern SerialLogger Logger;

#endif // SERIALLOGGER_HPP
