#!/bin/bash

# This utility script is to untrack changes in the IoT configuration header file.
# This is useful because we do not want to store sensitive data in it.
# If it is untracked, then we can freely modify the values.
git update-index --assume-unchanged ../project/src/IoTConfiguration.h
