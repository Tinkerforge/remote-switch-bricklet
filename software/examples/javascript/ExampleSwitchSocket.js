var IPConnection = require('Tinkerforge/IPConnection');
var BrickletRemoteSwitch = require('Tinkerforge/BrickletRemoteSwitch');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'jKy';// Change to your UID

var ipcon = new IPConnection();// Create IP connection
var rs = new BrickletRemoteSwitch(UID, ipcon);// Create device object

ipcon.connect(HOST, PORT,
    function(error) {
        console.log('Error: '+error);        
    }
);// Connect to brickd

// Don't use device before ipcon is connected
ipcon.on(IPConnection.CALLBACK_CONNECTED,
    function(connectReason) {
        // Switch socket with house code 17 and receiver code 1 on.
        // House code 17 is 10001 in binary (least-significant bit first)
        // and means that the DIP switches 1 and 5 are on and 2-4 are off.
        // Receiver code 1 is 10000 in binary (least-significant bit first)
        // and means that the DIP switch A is on and B-E are off.
        rs.switchSocketA(17, 1, BrickletRemoteSwitch.SWITCH_TO_ON);       
    }
);

console.log("Press any key to exit ...");
process.stdin.on('data',
    function(data) {
        ipcon.disconnect();
        process.exit(0);
    }
);

