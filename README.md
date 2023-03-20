# Thesis on Precision Air Quality Measurements Over Network via the MQTT Protocol using ESP32 MCUs

This project aims to create an expandable smart net of air quality sensors. I wish to provide firmware, a backend solution for data processing, communication and a hardware prototype.

In theory almost all the hardware, software and infrastructure can be swapped out if one complies with the interfaces described in the project, but due to the nature and complexity of the system, some might not integrate as well.

## Development tools used

- [Visual Studio Code](https://code.visualstudio.com/)
- [Arduino Expansion Pack](https://marketplace.visualstudio.com/items?itemName=mpty.pack-arduino)
- [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
- [IoT Telemetry Simulator](https://github.com/azure-samples/iot-telemetry-simulator/tree/master/)

## Libraries used in firmware

The following libraries and board definition(s) should be installed via the Arduino framework.

The versions referenced here are guaranteed to compile and function as expected, but you can try upgrading them if you want to, however it is possible that between versions there might be incompatibility. It is also possible that if you wish to build and flash firmware which communicates with a cloud platform with Azure, you might have to update the cloud configuration if not using the Azure templates in this repository. Always consult the documentation properly.

- [esp32 by Espressif Systems](https://github.com/espressif/arduino-esp32) (version 2.0.7) (Board definition for Arduino)
- [DHT sensor library by Adafruit](https://github.com/adafruit/DHT-sensor-library) (version 1.4.4)
- [ArduinoJSON](https://github.com/bblanchon/ArduinoJson) (version 6.21.0)
- [Adafruit MCP3008 by Adafruit](https://github.com/adafruit/Adafruit_MCP3008) (version 1.3.1)
- [Azure SDK for C by Microsoft Corporation](https://github.com/Azure/azure-sdk-for-c) (version 1.1.3)

## Useful links

### Containerization

- [Docker Compose Syntax: Volume or Bind Mount?](https://maximorlov.com/docker-compose-syntax-volume-or-bind-mount/): For configuring the telemetry simulator.
- [VSCode Remote Development Docoumentation](https://code.visualstudio.com/api/advanced-topics/remote-extensions#architecture-and-extension-types): For developing remotely or on immutable systems.