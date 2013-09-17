using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change to your UID

	static void Main() 
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletRemoteSwitch rs = new BrickletRemoteSwitch(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Switch socket with house code 42 and receiver code 23 on
		rs.SwitchSocket(42, 23, BrickletRemoteSwitch.SWITCH_TO_ON);

		System.Console.WriteLine("Press key to exit");
		System.Console.ReadKey();
	}
}
