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

        ' Switch socket with house code 17 and receiver code 16 on.
        ' House code 17 is 10001 in binary and means that the
        ' DIP switches 1 and 5 are on and 2-4 are off.
        ' Receiver code 16 is 10000 in binary and means that the
        ' DIP switches A is on and B-E are off.
        rs.SwitchSocket(17, 16, BrickletRemoteSwitch.SWITCH_TO_ON)

        System.Console.WriteLine("Press key to exit")
        System.Console.ReadKey()
        ipcon.Disconnect()
    End Sub
End Module
