# Android Controllable Lights

Developed by Thomas Jespersen, TKJ Electronics 2013
Modified for simple RGB strips by probono 2013

The code is released under the GNU General Public License.

This is the code is for controlling a simple RGB LED strip (not WS2801 based) with your Arduino using your Android phone with Bluetooth as remote control.

You can check out a video demonstration of the project here: <http://www.youtube.com/watch?v=pZSADQihV_Y> but please note that a simple RGB strip you can only modify the entire LED strip at once, so not all effects are available.

For more information see the blog post at <http://blog.tkjelectronics.dk/>.

## Commands

The first byte of a command package is the command byte, descriping the command of the package. The next bytes are all parameters as described below. Each parameter is terminated with a ‘;’ character, also when only one parameter is needed.

  * 0x0F = Color command [Parameters: 3 bytes of 3 char each]
  * 0x01 = Add fade color [Parameters: 3 bytes of 3 char each]
  * 0x02 = Reset fade colors [Parameters: None]
  * 0x03 = Set speed delay  [Parameters: 1 integer of 5 chars]
  * 0x04 = Enable Fade effect [Parameters: None]
  * 0x05 = Add Snap color [Parameters: 3 bytes of 3 char each]
  * 0x06 = Reset snap colors [Parameters: None]
  * 0x07 = Enable Snap effect [Parameters: None]
  * 0x08 = Add Running color (requires WS2801, hence disabled here) [Parameters: 3 bytes of 3 char each]
  * 0x09 = Reset Running colors (requires WS2801, hence disabled here) [Parameters: None]
  * 0x0A = Enable Running effect (requires WS2801, hence disabled here) [Parameters: None]
  * 0x0B = Add Running Fade color (requires WS2801, hence disabled here) [Parameters: 3 bytes of 3 char each]
  * 0x0C = Reset Running Fade colors (requires WS2801, hence disabled here) [Parameters: None]
  * 0x0D = Enable Running Fade effect (requires WS2801, hence disabled here) [Parameters: None]
  * 0x0E = Disable any effect [Parameters: None]
  * 0x1A = Invalidate EEProm

I can successfully send commands using
```
stty -F /dev/ttyUSB0 speed 57600  cs8 -cstopb -parenb -echo # Configure serial connection
cat /dev/ttyUSB0 & # Otherwise sending does NOT work!
echo -en '0 ' > /dev/ttyUSB0 # Reset the Arduino
sleep 2
echo -en '\x0f255;255;255;' > /dev/ttyUSB0 # Set all white
sleep 1 
echo -en '\x0f255;000;000;' > /dev/ttyUSB0 # Set all red
sleep 1 
echo -en '\x0f000;255;000;' > /dev/ttyUSB0 # Set all green
sleep 1 
echo -en '\x0f000;000;255;' > /dev/ttyUSB0 # Set all blue
killall cat # Do not forget this

When the app sends the 0x0f "white" command it looks like this:
00000220  32 3b 0f 32 35 32 3b 32  35 32 3b 32 35 32 3b 0f  |2;.252;252;252;.|
00000230  32 35 32 3b 32 35 32 3b  32 35 32 3b 0f 32 35 32  |252;252;252;.252|
00000240  3b 32 35 32 3b 32 35 32  3b 0f 32 35 32 3b 32 35  |;252;252;.252;25|
00000250  32 3b 32 35 32 3b 0f 32  35 32 3b 32 35 32 3b 32  |2;252;.252;252;2|
00000260  35 32 3b 0f 32 35 32 3b  32 35 32 3b 32 35 32 3b  |52;.252;252;252;|
00000270  0f 32 35 32 3b 32 35 32  3b 32 35 32 3b 0f 32 35  |.252;252;252;.25|

When the app sends the 0x0f "black" command it looks like this:
00000310  3b 30 30 30 3b 30 30 30  3b 0f 30 30 30 3b 30 30  |;000;000;.000;00|
00000320  30 3b 30 30 30 3b 0f 30  30 30 3b 30 30 30 3b 30  |0;000;.000;000;0|
00000330  30 30 3b 0f 30 30 30 3b  30 30 30 3b 30 30 30 3b  |00;.000;000;000;|
00000340  0f 30 30 30 3b 30 30 30  3b 30 30 30 3b 0f 30 30  |.000;000;000;.00|
00000350  30 3b 30 30 30 3b 30 30  30 3b 0f 30 30 30 3b 30  |0;000;000;.000;0|
00000360  30 30 3b 30 30 30 3b 0f  30 30 30 3b 30 30 30 3b  |00;000;.000;000;|

When the app sends the 0x03 "set speed delay" command to 100 it looks like this:
00000000  03 30 30 31 30 30 3b 03  30 30 31 30 30 3b 03 30  |.00100;.00100;.0|

```
