module.exports = async function (context, telemetryMessages) {

    const MILISEC_FACTOR = 24 * 60 * 60 * 1000;

    context.log(`JavaScript eventhub trigger function called for message array: ${JSON.stringify(telemetryMessages)}`);

    context.bindings.telemetryTableStorage = [];

    telemetryMessages.forEach((message, index) => {

        context.log(`Processing message: ${JSON.stringify(message)}`);

        let currentDate = Date.now();
        let partitionKey = Math.floor(currentDate / MILISEC_FACTOR) + "";
        let rowKey = currentDate + "";

        let messageJsonObject = JSON.parse(message);

        let preparedJsonObject = {
            PartitionKey: partitionKey,
            RowKey: rowKey,
            OriginalMessage: JSON.stringify(message),
            DeviceID: messageJsonObject.deviceId,
            Temperature: messageJsonObject.temperature,
            Humidity: messageJsonObject.humidity,
            CoPPM: messageJsonObject.co_level,
            DeviceTime: messageJsonObject.device_time
        };

        context.log(`Trying to send: ${JSON.stringify(preparedJsonObject)}`);

        context.bindings.telemetryTableStorage.push(preparedJsonObject);
    });

};
