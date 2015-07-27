#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_remote_switch'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change to your UID

ipcon = IPConnection.new # Create IP connection
rs = BrickletRemoteSwitch.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Switch on a type A socket with house code 17 and receiver code 1.
# House code 17 is 10001 in binary (least-significant bit first)
# and means that the DIP switches 1 and 5 are on and 2-4 are off.
# Receiver code 1 is 10000 in binary (least-significant bit first)
# and means that the DIP switch A is on and B-E are off.
rs.switch_socket_a 17, 1, BrickletRemoteSwitch::SWITCH_TO_ON

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
