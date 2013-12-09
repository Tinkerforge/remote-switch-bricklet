
#include <stdio.h>

#include "ip_connection.h"
#include "bricklet_remote_switch.h"

#define HOST "localhost"
#define PORT 4223
#define UID "XYZ" // Change to your UID

int main() {
	// Create IP connection
	IPConnection ipcon;
	ipcon_create(&ipcon);

	// Create device object
	RemoteSwitch rs;
	remote_switch_create(&rs, UID, &ipcon); 

	// Connect to brickd
	if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
		fprintf(stderr, "Could not connect\n");
		exit(1);
	}
	// Don't use device before ipcon is connected

	// Switch socket with house code 17 and receiver code 16 on.
	// House code 17 is 10001 in binary and means that the
	// DIP switches 1 and 5 are on and 2-4 are off.
	// Receiver code 16 is 10000 in binary and means that the
	// DIP switches A is on and B-E are off.
	remote_switch_switch_socket(&rs, 17, 16, REMOTE_SWITCH_SWITCH_TO_ON);

	printf("Press key to exit\n");
	getchar();
	ipcon_destroy(&ipcon);
}
