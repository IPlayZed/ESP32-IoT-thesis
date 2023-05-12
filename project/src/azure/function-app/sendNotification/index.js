module.exports = async function (context, telemetryMessages) {

    const NTFY_URL = 'https://ntfy.sh/co-sensors-alerts';
    context.log(`JavaScript eventhub trigger function called for message array: ${JSON.stringify(telemetryMessages)}`);

    telemetryMessages.forEach((message, index) => {

        context.log(`Processing message: ${JSON.stringify(message)}`);

        let messageJsonObject = JSON.parse(message);
        
        let ppmLevel = parseFloat(messageJsonObject.co_level);
        const NTFY_BODY = `Carbon-monoxide PPM level is ${ppmLevel} for device ${messageJsonObject.deviceId}`;
        
        let title = null;
        let priority = null;
        let tags = null;

        if(ppmLevel < 1)
        {
            title = 'Healthy levels of carbon-monoxide!';
            priority = 'min';
            tags = '+1';
        }
        else if(ppmLevel < 50)
        {
            title = 'Health risk on medium to long term.';
            priority = 'low';
            tags = 'warning';
        }
        else if(ppmLevel < 200)
        {
            title = 'High level of CO! Heart and brain damage possible in a few hours!';
            priority = 'default';
            tags = 'loudspeaker';
        }
        else if(ppmLevel < 800)
        {
            title = 'EVACUATE AREA! DEATH IN A FEW HOURS!';
            priority = 'high';
            tags = 'rotating_light,skull';
        }
        else
        {
            title = 'EVACUATE AREA! DEATH IN LESS THAN 1 HOUR!';
            priority = 'max';
            tags = 'no_entry,rotating_light,skull';
        }

        let httpsHeaders = {
            'Title': title,
            'Priority': priority,
            'Tags': tags
        };
        
        let httpMessage = {
            method: 'POST',
            body: NTFY_BODY,
            headers: httpsHeaders
        }

        context.log(`Trying to send HTTP message: ${JSON.stringify(httpMessage)} to ${NTFY_URL}`);

        fetch(NTFY_URL, httpMessage);
    });

};
