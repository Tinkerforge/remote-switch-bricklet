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

    # Switch socket with house code 42 and receiver code 23 on
    rs.switch_socket(42, 23, RemoteSwitch.SWITCH_TO_ON)

    raw_input('Press key to exit\n') # Use input() in Python 3
