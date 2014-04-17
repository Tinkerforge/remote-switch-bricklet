function octave_example_switch_socket()
    more off;
    
    HOST = "localhost";
    PORT = 4223;
    UID = "jzG"; % Change to your UID

    ipcon = java_new("com.tinkerforge.IPConnection"); % Create IP connection
    rs = java_new("com.tinkerforge.BrickletRemoteSwitch", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Switch socket with house code 17 and receiver code 1 on.
    % House code 17 is 10001 in binary (least-significant bit first)
    % and means that the DIP switches 1 and 5 are on and 2-4 are off.
    % Receiver code 1 is 10000 in binary (least-significant bit first)
    % and means that the DIP switch A is on and B-E are off.
    rs.switchSocketA(0, 31, rs.SWITCH_TO_ON);

    input("Press any key to exit...\n", "s");
    ipcon.disconnect();
end
