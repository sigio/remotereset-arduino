RemoteReset-Arduino
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

Goals
-----

* Connecting to the serial port should NOT trigger any resets or boots
* Resetting the arduino should not trigger any resets or boots
* Hosts can be defined with a name, and pinnumbers for reset and pwrbtn
* Command to query poweron status of a host / hosts
* Commands to trigger reset or power
* Online help

Later
-----

* Login with a password
* Logout command / Logout after a timeout
* Ethernet support
* Configuration stored in eeprom, commands to change/update/delete config
* Configurable short/long pulse times

Schematic
---------

Any arduino should do, D2 ... D12 should be available to connect to reset
and pwrbtn lines. D13 maybe (untested) and A0-A6 (untested)
Arduino Mega should allow even more connections

Connect the pins on the arduino with the reset and pwrbtn pins on the mainboard.
Connect the GND on the arduino with the GND on the mainboard (pin next to reset and pwrbtn are both ground)
