#!/usr/bin/perl

use Tinkerforge::IPConnection;
use Tinkerforge::BrickletRemoteSwitch;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Remote Switch Bricklet

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $rs = Tinkerforge::BrickletRemoteSwitch->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Switch on a type A socket with house code 17 and receiver code 1.
# House code 17 is 10001 in binary (least-significant bit first)
# and means that the DIP switches 1 and 5 are on and 2-4 are off.
# Receiver code 1 is 10000 in binary (least-significant bit first)
# and means that the DIP switch A is on and B-E are off.
$rs->switch_socket_a(17, 1, $rs->SWITCH_TO_ON);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
