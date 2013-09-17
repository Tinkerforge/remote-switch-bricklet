program ExampleSwitchSocket;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickletRemoteSwitch;

type
  TExample = class
  private
    ipcon: TIPConnection;
    rs: TBrickletRemoteSwitch;
  public
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'XYZ'; { Change to your UID }

var
  e: TExample;

procedure TExample.Execute;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  rs := TBrickletRemoteSwitch.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Switch socket with house code 42 and receiver code 23 on }
  rs.SwitchSocket(42, 23, BRICKLET_REMOTE_SWITCH_SWITCH_TO_ON);

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy;
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
