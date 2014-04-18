# Android Controllable Lights

Developed by Thomas Jespersen, TKJ Electronics 2013
Modified for simple RGB strips by probono 2013

The code is released under the GNU General Public License.

This is the code is for controlling a simple RGB LED strip (not WS2801 based) with your Arduino using your Android phone with Bluetooth as remote control.

You can check out a video demonstration of the project here: <http://www.youtube.com/watch?v=pZSADQihV_Y> but please note that a simple RGB strip you can only modify the entire LED strip at once, so not all effects are available.

For more information see the blog post at <http://blog.tkjelectronics.dk/>.

## Commands
  * 0x0F = Color command
  * 0x01 = Add fade color
  * 0x02 = Reset fade colors
  * 0x03 = Set speed delay  
  * 0x04 = Enable Fade effect
  * 0x05 = Add Snap color
  * 0x06 = Reset snap colors
  * 0x07 = Enable Snap effect
  * 0x08 = Add Running color (requires WS2801, hence disabled here)
  * 0x09 = Reset Running colors (requires WS2801, hence disabled here)
  * 0x0A = Enable Running effect (requires WS2801, hence disabled here)
  * 0x0B = Add Running Fade color (requires WS2801, hence disabled here)
  * 0x0C = Reset Running Fade colors (requires WS2801, hence disabled here)
  * 0x0D = Enable Running Fade effect (requires WS2801, hence disabled here)
  * 0x0E = Disable any effect
  * 0x1A = Invalidate EEProm
