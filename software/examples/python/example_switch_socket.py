#!/usr/bin/env python
# -*- coding: utf-8 -*-  

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change to your UID

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_remote_switch import RemoteSwitch

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    rs = RemoteSwitch(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Switch socket with house code 17 and receiver code 16 on.
    # House code 17 is 10001 in binary and means that the
    # DIP switches 1 and 5 are on and 2-4 are off.
    # Receiver code 16 is 10000 in binary and means that the
    # DIP switches E is on and A-D are off.
    rs.switch_socket(17, 16, RemoteSwitch.SWITCH_TO_ON)

    raw_input('Press key to exit\n') # Use input() in Python 3
