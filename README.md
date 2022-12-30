# computer-desk
Arduino code to control an LED strip that is built into a computer desk via an IR remote and HomeKit.

![Main](https://github.com/ajschwieterman/computer-desk/blob/main/main.jpg)

# Hardware
- [Adafruit Feather HUZZAH ESP8266](https://www.adafruit.com/product/2821)
- [Adafruit NeoPixels](https://www.adafruit.com/product/1138)
- [IR Receiver Sensor](https://www.adafruit.com/product/157)
- [IR Remote](https://www.amazon.com/SUPERNIGHT-Remote-Controller-Wireless-Control/dp/B00AF5YOK2/ref=asc_df_B00AF5YOK2/?tag=hyprod-20&linkCode=df0&hvadid=233903308735&hvpos=&hvnetw=g&hvrand=3245025783190143450&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9017486&hvtargid=pla-393085054271&psc=1)

# Schematic
Below is the wiring that I used to supply 5V of power to the NeoPixels and controller.  I merged a USB-A male connector and micro-USB male connector and added an in-line capacitor.
![Main](https://github.com/ajschwieterman/computer-desk-lights/blob/main/power.png)

Below is the circuit diagram for the IR receiver sensor and NeoPixels.
![Main](https://github.com/ajschwieterman/computer-desk-lights/blob/main/schematic.png)

# Required Arduino libraries
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) v1.10.7
- [HomeKit-ESP8266](https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266) v1.2.0
- [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) v2.8.4
