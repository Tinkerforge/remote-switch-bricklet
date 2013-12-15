#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# Switch socket with house code 17 and receiver code 16 on.
# House code 17 is 10001 in binary and means that the
# DIP switches 1 and 5 are on and 2-4 are off.
# Receiver code 16 is 10000 in binary and means that the
# DIP switch E is on and A-D are off.
tinkerforge call remote-switch-bricklet $uid switch-socket 17 16 on
