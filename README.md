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

Most likely I can send commands using
```
stty -F /dev/ttyUSB0 speed 57600  cs8 -cstopb -parenb -echo
echo -en '0 ' > /dev/ttyUSB0 # Resets the device successfully into the bootloader
echo -en '\x15255;000;000;' > /dev/ttyUSB0 # does not appear to do anything
```
