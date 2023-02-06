//'use strict'

//const PARTITION_KEY = "KEY_PARTITION";
//const ROW_KEY = "KEY_ROW";
//const MESSAGE_COL = "COL_MESSAGE"
const MILISEC_FACTOR = 24 * 60 * 60 * 1000;


module.exports = async function (context, eventHubMessages) {
    
    context.log('Message recieved from event hub: ' + JSON.stringify(eventHubMessages));

    let currentDate = Date.now();
    let partitionKey = Math.floor(currentDate / MILISEC_FACTOR) + '';
    let rowKey = currentDate + '';
    context.log(`currentDate: ${currentDate}; partitionKey: ${partitionKey}; rowKey: ${rowKey}`);

    // the "coSensorsDataTableParameterName" is configured in the Integration GUI menupoint in the output as the table parameter name
    context.bindings.coSensorsDataTableParameterName = {
        PartitionKey: partitionKey,
        RowKey: rowKey,
        Message: JSON.stringify(eventHubMessages)
    };

    eventHubMessages.forEach((message, index) => {
        context.log(`Processed message ${message}`);
    });

    context.done(); // TODO: Find out if this is needed at all.
};
