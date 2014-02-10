#!/usr/bin/perl  

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRemoteSwitch;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'ufe'; # Change to your UID

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs = Tinkerforge::BrickletRemoteSwitch->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Switch socket with house code 42 and receiver code 23 on
$rs->switch_socket(42, 23, $rs->SWITCH_TO_ON);

print "\nPress any key to exit...\n";
<STDIN>;
$ipcon->disconnect();

