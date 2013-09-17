
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

	// Switch socket with house code 42 and receiver code 23 on
	remote_switch_switch_socket(&rs, 42, 23, REMOTE_SWITCH_SWITCH_TO_ON);

	printf("Press key to exit\n");
	getchar();
	ipcon_destroy(&ipcon);
}
