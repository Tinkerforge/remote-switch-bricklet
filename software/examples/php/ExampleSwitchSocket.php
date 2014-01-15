<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRemoteSwitch.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRemoteSwitch;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change to your UID

$ipcon = new IPConnection(); // Create IP connection
$rs = new BrickletRemoteSwitch(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Switch socket with house code 17 and receiver code 1 on.
// House code 17 is 10001 in binary (least-significant bit first)
// and means that the DIP switches 1 and 5 are on and 2-4 are off.
// Receiver code 1 is 10000 in binary (least-significant bit first)
// and means that the DIP switch A is on and B-E are off.
$rs->switchSocketA(17, 1, BrickletRemoteSwitch::SWITCH_TO_ON);

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));

?>
