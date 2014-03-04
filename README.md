RemoteReset-Arduino, enhanced by eXtremeSHOK
-------------------

Arduino sketch to toggle reset and powerbuttons on standard ATX
mainboards.

Connecting the mainboard reset and pwrbtn pins to arduino digital pins
will allow you to remotely boot / shutdown or reset a PC.

By reading the state of the reset line, we can see if the system is
on or off and by reading the state of the pwrbtn line we can see if
the powersupply is turned on / power is present.

Toggling the reset line will reset the system. Toggling the pwrbtn line
will either stop (shutdown) or start the system.

Easiest is to attach the arduino (nano is my preferred platform) to
a USB port on another box (OpenWRT, other pc, etc).

Done
-----

* Connecting to the serial port should NOT trigger any resets or boots
* Resetting the arduino should not trigger any resets or boots
* Hosts can be defined with a name, and pinnumbers for reset and power
* Command to query poweron status of a host / hosts (state)
* Commands to trigger reset or power
* Online help
* Configuration stored in eeprom, commands to change/update/delete config

eXtremeSHOK
-----
* Configurable short/long pulse times
* Toggle reset switch on All configs (resetall)
* Toggle power switch on All configs (powerall)
* Long-Press power switch on All configs (forceall)
* Erase All configs (eraseall)
* Check All Power state ( status )( stateall )
* Enable csv output for prasing of status ( status 1 )
* Power OFF machines which are on  ( poweroffon )
* Power ON Machines which are off ( poweronoff )

Later
-----

* Login with a password
* Logout command / Logout after a timeout
* Ethernet support


Schematic
---------

Any arduino should do, D2 ... D12 should be available to connect to reset
and pwrbtn lines. D13 maybe (untested) and A0-A6 (untested)
Arduino Mega should allow even more connections

Connect the pins on the arduino with the reset and pwrbtn pins on the mainboard.
Connect the GND on the arduino with the GND on the mainboard (pin next to reset and pwrbtn are both ground)
