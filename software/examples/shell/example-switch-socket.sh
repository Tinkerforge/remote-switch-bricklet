#!/bin/sh
# connects to localhost:4223 by default, use --host and --port to change it

# change to your UID
uid=XYZ

# Switch socket with house code 42 and receiver code 23 on
tinkerforge call remote-switch-bricklet $uid switch-socket 42 23 on 
