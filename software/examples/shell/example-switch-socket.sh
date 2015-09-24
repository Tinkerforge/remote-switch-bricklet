#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change to your UID

# Switch on a type A socket with house code 17 and receiver code 1.
# House code 17 is 10001 in binary (least-significant bit first)
# and means that the DIP switches 1 and 5 are on and 2-4 are off.
# Receiver code 1 is 10000 in binary (least-significant bit first)
# and means that the DIP switch A is on and B-E are off.
tinkerforge call remote-switch-bricklet $uid switch-socket-a 17 1 on
