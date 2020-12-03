# computer-desk
Arduino code to control an LED strip that is built into a computer desk via an IR remote.
![Main](https://github.com/ajschwieterman/computer-desk/blob/main/main.jpg)

# Hardware
- [Adafruit NeoPixels](https://www.adafruit.com/product/1138?length=1)
- [IR Receiver Sensor](https://www.adafruit.com/product/157)
- [IR Remote](https://www.amazon.com/SUPERNIGHT-Remote-Controller-Wireless-Control/dp/B00AF5YOK2/ref=asc_df_B00AF5YOK2/?tag=hyprod-20&linkCode=df0&hvadid=233903308735&hvpos=&hvnetw=g&hvrand=3245025783190143450&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9017486&hvtargid=pla-393085054271&psc=1)
- [Teensy 3.2](https://www.adafruit.com/product/2756)

# Schematic
Below is the wiring that I used to supply 5V of power to the NeoPixels and controller.  I merged a USB-A male connector and micro-USB male connector and added an in-line capacitor.
![Main](https://github.com/ajschwieterman/computer-desk/blob/main/power.png)

Below is the circuit diagram for the IR receiver sensor and NeoPixels.
![Main](https://github.com/ajschwieterman/computer-desk/blob/main/schematic.png)

# Required Arduino libraries
- [FastLED v3.3.0](https://github.com/FastLED/FastLED)
- [IRRemote v2.8.0](https://github.com/z3t0/Arduino-IRremote)
