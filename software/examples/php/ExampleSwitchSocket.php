<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletRemoteSwitch.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletRemoteSwitch;

$host = 'localhost';
$port = 4223;
$uid = 'XYZ'; // Change to your UID

$ipcon = new IPConnection(); // Create IP connection
$rs = new BrickletRemoteSwitch($uid, $ipcon); // Create device object

$ipcon->connect($host, $port); // Connect to brickd
// Don't use device before ipcon is connected

// Switch socket with house code 17 and receiver code 16 on.
// House code 17 is 10001 in binary and means that the
// DIP switches 1 and 5 are on and 2-4 are off.
// Receiver code 16 is 10000 in binary and means that the
// DIP switches A is on and B-E are off.
$rs->switchSocket(17, 16, BrickletRemoteSwitch::SWITCH_TO_ON);

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));

?>
