use std::{error::Error, io};

use tinkerforge::{ip_connection::IpConnection, remote_switch_bricklet::*};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Remote Switch Bricklet.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let rs = RemoteSwitchBricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    // Switch on a type A socket with house code 17 and receiver code 1.
    // House code 17 is 10001 in binary (least-significant bit first)
    // and means that the DIP switches 1 and 5 are on and 2-4 are off.
    // Receiver code 1 is 10000 in binary (least-significant bit first)
    // and means that the DIP switch A is on and B-E are off.
    rs.switch_socket_a(17, 1, REMOTE_SWITCH_BRICKLET_SWITCH_TO_ON).recv()?;

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
