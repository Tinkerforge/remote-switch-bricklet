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

# Switch socket with house code 42 and receiver code 23 on
rs.switch_socket 42, 23, BrickletRemoteSwitch::SWITCH_TO_ON

puts 'Press key to exit'
$stdin.gets
