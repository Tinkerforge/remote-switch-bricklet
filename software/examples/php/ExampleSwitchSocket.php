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

// Switch socket with house code 42 and receiver code 23 on
$rs->switchSocket(42, 23, BrickletRemoteSwitch::SWITCH_TO_ON);

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));

?>
