#!/bin/bash

# This utility script tracks the IoT configuraton header file.
# This is useful if one wants to change not the value, but the
# structure of the configuraton file.
git update-index --no-assume-unchanged ../project/src/Main/IoTConfiguration.h
