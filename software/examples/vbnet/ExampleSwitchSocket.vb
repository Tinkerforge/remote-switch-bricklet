Imports Tinkerforge

Module ExampleSwitchSocket
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change to your UID

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim rs As New BrickletRemoteSwitch(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Switch socket with house code 42 and receiver code 23 on
		rs.SwitchSocket(42, 23, BrickletRemoteSwitch.SWITCH_TO_ON) 

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadKey()
        ipcon.Disconnect()
    End Sub
End Module
