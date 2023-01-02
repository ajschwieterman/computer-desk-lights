#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>

#define EEPROM_STARTING_ADDRESS   1500
#define INACTIVE_DIY_COLOR_INDEX  -1
#define IR_RECEIVER_PIN           12
#define NEOPIXEL_COUNT            65
#define NEOPIXEL_PIN              14
#define BLUE_DECREASE_CODE        0xFF48B7
#define BLUE_INCREASE_CODE        0xFF6897
#define BRIGHTNESS_DECREASE_CODE  0xFFBA45
#define BRIGHTNESS_INCREASE_CODE  0xFF3AC5
#define DIY_KEY_1_CODE            0xFF30CF
#define DIY_KEY_2_CODE            0xFFB04F
#define DIY_KEY_3_CODE            0xFF708F
#define DIY_KEY_4_CODE            0xFF10EF
#define DIY_KEY_5_CODE            0xFF906F
#define DIY_KEY_6_CODE            0xFF50AF
#define GREEN_DECREASE_CODE       0xFF8877
#define GREEN_INCREASE_CODE       0xFFA857
#define ON_OFF_CODE               0xFF02FD
#define RED_DECREASE_CODE         0xFF08F7
#define RED_INCREASE_CODE         0xFF28D7
#define STATIC_BLUE_CODE          0xFFA25D
#define STATIC_BLUE_WHITE         0xFFF807
#define STATIC_BROWN_CODE         0xFF58A7
#define STATIC_CYAN_CODE          0xFF8A75
#define STATIC_DARK_BLUE_CODE     0xFF926D
#define STATIC_DARK_YELLOW_CODE   0xFF0AF5
#define STATIC_GREEN_CODE         0xFF9A65
#define STATIC_GREEN_WHITE_CODE   0xFFD827
#define STATIC_LIGHT_BLUE_CODE    0xFFB847
#define STATIC_LIGHT_GREEN_CODE   0xFFAA55
#define STATIC_LIGHT_YELLOW_CODE  0xFF18E7
#define STATIC_LYONS_BLUE_CODE    0xFFB24D
#define STATIC_MILK_WHITE_CODE    0xFF12ED
#define STATIC_ORANGE_CODE        0xFF2AD5
#define STATIC_PURPLE_CODE        0xFF7887
#define STATIC_RED_CODE           0xFF1AE5
#define STATIC_SKY_BLUE_CODE      0xFF9867
#define STATIC_WHITE_CODE         0xFF22DD
#define STATIC_WHITE_PINK_CODE    0xFF32CD
#define STATIC_YELLOW_CODE        0xFF38C7
#define WIFI_SSID                 ""
#define WIFI_PASSWORD             ""

/* Define color spaces */
typedef struct {
  float hue;
  float saturation;
  float value;
} hsv_t;
typedef struct {
  float red;
  float green;
  float blue;
} rgb_t;

/* Define hardware */
IRrecv irReceiver(IR_RECEIVER_PIN);
Adafruit_NeoPixel neopixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* Define HomeKit configuration */
extern "C" homekit_server_config_t homekitConfiguration;
extern "C" homekit_characteristic_t homekitOnOffCharacteristic;
extern "C" homekit_characteristic_t homekitBrightnessCharacteristic;
extern "C" homekit_characteristic_t homekitSaturationCharacteristic;
extern "C" homekit_characteristic_t homekitHueCharacteristic;

/* Other variables */
int activeDiyColorIndex = INACTIVE_DIY_COLOR_INDEX;
hsv_t diyColors[6];
decode_results irDecoder;
bool updateLeds = false;

void setup() {
  /* Start the serial monitor */
  // Serial.begin(115200);
  /* Start the EEPROM service */
  EEPROM.begin(EEPROM_STARTING_ADDRESS + sizeof(diyColors));
  EEPROM.get(EEPROM_STARTING_ADDRESS, diyColors);
  // resetDiyColors();
  /* Start the IR Receiver service */
  irReceiver.enableIRIn();
  /* Start the NeoPixel service */
  neopixels.begin();
  neopixels.clear();
  neopixels.show();
  /* Start the Wi-Fi service */
  WiFi.disconnect();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (!WiFi.isConnected()) {
		delay(100);
	}
  /* Start the HomeKit service */
  homekitOnOffCharacteristic.setter = setOnOffState;
  homekitBrightnessCharacteristic.setter = setBrightness;
  homekitSaturationCharacteristic.setter = setSaturation;
  homekitHueCharacteristic.setter = setHue;
  // homekit_storage_reset();
	arduino_homekit_setup(&homekitConfiguration);
}

//------------------------------------------------------------------------------------------------------------------------

void loop() {
  /* Monitor any inputs from the IR receiver */
  ir_receiver_loop();
  /* Monitor any changes from HomeKit */
  arduino_homekit_loop();
  /* Change the color of the LEDs after receiving the inputs */
  neopixels_loop();
}

//------------------------------------------------------------------------------------------------------------------------

/**
 * The IR receiver loop monitors for any messages received from the IR remote. Once a valid message is received, the 
 * NeoPixels are updated based on the what message was received. The remote has buttons that can change the brightness 
 * and color of the LEDs through static buttons or provide custom colors with variable RGB values.
 */
void ir_receiver_loop() {
  if (irReceiver.decode(&irDecoder)) {
    if (irDecoder.decode_type == NEC) {
      /* Change the LEDs on/off state if the power button was pressed */
      if (irDecoder.value == ON_OFF_CODE) {
        setOnOffState(!homekitOnOffCharacteristic.value.bool_value);
      }
      /* Change the LEDs color if they are on */
      if (homekitOnOffCharacteristic.value.bool_value) {
        switch(irDecoder.value) {
          case BLUE_DECREASE_CODE:
            adjustDiyColor(0.0f, 0.0f, -4.0f);
            break;
          case BLUE_INCREASE_CODE:
            adjustDiyColor(0.0f, 0.0f, 4.0f);
            break;
          case BRIGHTNESS_DECREASE_CODE:
            setBrightness(max(homekitBrightnessCharacteristic.value.int_value - 10, 0));
            break;
          case BRIGHTNESS_INCREASE_CODE:
            setBrightness(min(homekitBrightnessCharacteristic.value.int_value + 10, 100));
            break;
          case DIY_KEY_1_CODE:
            setDiyColor(0);
            break;
          case DIY_KEY_2_CODE:
            setDiyColor(1);
            break;
          case DIY_KEY_3_CODE:
            setDiyColor(2);
            break;
          case DIY_KEY_4_CODE:
            setDiyColor(3);
            break;
          case DIY_KEY_5_CODE:
            setDiyColor(4);
            break;
          case DIY_KEY_6_CODE:
            setDiyColor(5);
            break;
          case GREEN_DECREASE_CODE:
            adjustDiyColor(0.0f, -4.0f, 0.0f);
            break;
          case GREEN_INCREASE_CODE:
            adjustDiyColor(0.0f, 4.0f, 0.0f);
            break;
          case RED_DECREASE_CODE:
            adjustDiyColor(-4.0f, 0.0f, 0.0f);
            break;
          case RED_INCREASE_CODE:
            adjustDiyColor(4.0f, 0.0f, 0.0f);
            break;
          case STATIC_BLUE_CODE:
            setStaticColor(240.0f, 100.0f);
            break;
          case STATIC_BLUE_WHITE:
            setStaticColor(300.0f, 50.0f);
            break;
          case STATIC_BROWN_CODE:
            setStaticColor(300.0f, 100.0f);
            break;
          case STATIC_CYAN_CODE:
            setStaticColor(165.0f, 100.0f);
            break;
          case STATIC_DARK_BLUE_CODE:
            setStaticColor(255.0f, 100.0f);
            break;
          case STATIC_DARK_YELLOW_CODE:
            setStaticColor(30.0f, 100.0f);
            break;
          case STATIC_GREEN_CODE:
            setStaticColor(120.0f, 100.0f);
            break;
          case STATIC_GREEN_WHITE_CODE:
            setStaticColor(180.0f, 12.5f);
            break;
          case STATIC_LIGHT_BLUE_CODE:
            setStaticColor(195.0f, 100.0f);
            break;
          case STATIC_LIGHT_GREEN_CODE:
            setStaticColor(150.0f, 100.0f);
            break;
          case STATIC_LIGHT_YELLOW_CODE:
            setStaticColor(60.0f, 100.0f);
            break;
          case STATIC_LYONS_BLUE_CODE:
            setStaticColor(270.0f, 100.0f);
            break;
          case STATIC_MILK_WHITE_CODE:
            setStaticColor(300.0f, 50.0f);
            break;
          case STATIC_ORANGE_CODE:
            setStaticColor(15.0f, 100.0f);
            break;
          case STATIC_PURPLE_CODE:
            setStaticColor(285.0f, 100.0f);
            break;
          case STATIC_RED_CODE:
            setStaticColor(0.0f, 100.0f);
            break;
          case STATIC_SKY_BLUE_CODE:
            setStaticColor(210.0f, 100.0f);
            break;
          case STATIC_WHITE_CODE:
            setStaticColor(0.0f, 0.0f);
            break;
          case STATIC_WHITE_PINK_CODE:
            setStaticColor(300.0f, 12.5f);
            break;
          case STATIC_YELLOW_CODE:
            setStaticColor(45.0f, 100.0f);
            break;
        }
      }
    }
    /* Prepare the IR receiver for a new code */
    irReceiver.resume();
  }
}

/**
 * Update the NeoPixels to the requested color, whether it was requested by HomeKit or the IR remote. This function uses 
 * the RGB color space to change the LED values, and HomeKit uses the HSV color space, so the conversion between the color
 * spaces are done here, as well.
 */
void neopixels_loop() {
  if (updateLeds) {
    uint32_t color = neopixels.Color(0, 0, 0);
    /* Set what the color of the LEDs will be */
    if (homekitOnOffCharacteristic.value.bool_value) {
      hsv_t hsv = {
        .hue = homekitHueCharacteristic.value.float_value,
        .saturation = homekitSaturationCharacteristic.value.float_value,
        .value = homekitBrightnessCharacteristic.value.int_value
      };
      rgb_t rgb = hsvToRgb(hsv);
      color = neopixels.Color(rgb.red, rgb.green, rgb.blue);
    }
    /* Send the update to the LEDs to show the new color */
    neopixels.clear();
    neopixels.fill(color, 0, NEOPIXEL_COUNT);
    neopixels.show();
    /* Once finished, reset the flag to update the LEDs on the next input change */
    updateLeds = false;
  }
}

//------------------------------------------------------------------------------------------------------------------------

/**
 * If a custom color is currently being displayed, this function is used to allow the user to adjust any of the 
 * RGB components of the color to their desire. Note that the color will only update if the brightness of the LEDs
 * does not change
 * 
 * @param r_delta   The amount to change the red component of the custom color
 * @param g_delta   The amount to change the green component of the custom color
 * @param b_delta   The amount to change the blue component of the custom color
 */
void adjustDiyColor(float r_delta, float g_delta, float b_delta) {
  /* Adjust the DIY color if it any of the DIY colors is currently displayed */
  if (activeDiyColorIndex != INACTIVE_DIY_COLOR_INDEX) {
    /* Save the current brightness */
    diyColors[activeDiyColorIndex].value = homekitBrightnessCharacteristic.value.int_value;
    /* Adjust the DIY color's RGB values */
    rgb_t rgb = hsvToRgb(diyColors[activeDiyColorIndex]);
    rgb.red = fmin(fmax(rgb.red + r_delta, 0.0f), 255.0f);
    rgb.green = fmin(fmax(rgb.green + g_delta, 0.0f), 255.0f);
    rgb.blue = fmin(fmax(rgb.blue + b_delta, 0.0f), 255.0f);
    /* Set and save the new DIY color if the brightness did not change */
    hsv_t hsv = rgbToHsv(rgb);
    if (hsv.value == diyColors[activeDiyColorIndex].value) {
      diyColors[activeDiyColorIndex] = hsv;
      setDiyColor(activeDiyColorIndex);
      writeDiyColorsToEeprom();
    }
  }
}

/**
 * Set the brightness of the LEDs. This is called either when HomeKit receives a request to change the brightness
 * or the user presses any of the brightness buttons on the IR remote.
 * 
 * @param brightness  The new brightness value of the LEDs
 */
void setBrightness(int brightness) {
  homekitBrightnessCharacteristic.value.int_value = brightness;
  updateLeds = true;
}

/**
 * Set the color of the LEDs to one of the six available custom colors. This is called when the user presses any 
 * of the DIY color buttons on the IR remote.
 * 
 * @param index   The index of the custom color
 */
void setDiyColor(int index) {
  homekitHueCharacteristic.value.float_value = diyColors[index].hue;
  homekitSaturationCharacteristic.value.float_value = diyColors[index].saturation;
  activeDiyColorIndex = index;
  updateLeds = true;
}

/**
 * Set the on/off state of the LEDs. This is called either when HomeKit receives a request to change the state
 * or the user presses the power button on the IR remote.
 * 
 * @param state   The new on/off state of the LEDs
 */
void setOnOffState(bool state) {
  homekitOnOffCharacteristic.value.bool_value = state;
  updateLeds = true;
}

/**
 * Set the color of the LEDs to a new color. This is called either when HomeKit changes the hue and/or saturation 
 * values of the color or the user presses any of the static color buttons on the IR remote.
 * 
 * @param hue         The new hue value of the color
 * @param saturation  The new saturation value of the color
 */
void setStaticColor(float hue, float saturation) {
  homekitHueCharacteristic.value.float_value = hue;
  homekitSaturationCharacteristic.value.float_value = saturation;
  activeDiyColorIndex = INACTIVE_DIY_COLOR_INDEX;
  updateLeds = true;
}

//------------------------------------------------------------------------------------------------------------------------

/**
 * Callback function called by HomeKit to set the brightness of the LEDs.
 * 
 * @param value   The new brightness value of the LEDs.
 */
void setBrightness(const homekit_value_t value) {
  setBrightness(value.int_value);
}

/**
 * Callback function called by HomeKit to set the hue of the LEDs.
 * 
 * @param value   The new hue value of the LEDs.
 */
void setHue(const homekit_value_t value) {
  setStaticColor(value.float_value, homekitSaturationCharacteristic.value.float_value);
}

/**
 * Callback function called by HomeKit to turn the LEDs either on or off.
 * 
 * @param value   The new on/off state of the LEDs
 */
void setOnOffState(const homekit_value_t value) {
  setOnOffState(value.bool_value);
}

/**
 * Callback function called by HomeKit to set the saturation of the LEDs.
 * 
 * @param value   The new saturation value of the LEDs.
 */
void setSaturation(const homekit_value_t value) {
  setStaticColor(homekitHueCharacteristic.value.float_value, value.float_value);
}

//------------------------------------------------------------------------------------------------------------------------

/**
 * Convert a HSV color to a RGB color.
 * 
 * @param hsv HSV color
 * @return    RGB color 
 * 
 * @link https://www.rapidtables.com/convert/color/hsv-to-rgb.html
 */
rgb_t hsvToRgb(hsv_t hsv) {
  rgb_t rgb;
  /* Convert range of HSV values from 0..100 to 0..1 (saturation and value) */
  float h = hsv.hue;
  float s = hsv.saturation / 100.0f;
  float v = hsv.value / 100.0f;
  /* Calculate helper variables for RGB values */
  float c = v * s;
  float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
  float m = v - c;
  /* Calculate RGB values */
  switch ((int)floorf(h / 60.0f)) {
    case 0:
    case 6:
      rgb.red = c;
      rgb.green = x;
      rgb.blue = 0.0f;
      break;
    case 1:
      rgb.red = x;
      rgb.green = c;
      rgb.blue = 0.0f;
      break;
    case 2:
      rgb.red = 0.0f;
      rgb.green = c;
      rgb.blue = x;
      break;
    case 3:
      rgb.red = 0.0f;
      rgb.green = x;
      rgb.blue = c;
      break;
    case 4:
      rgb.red = x;
      rgb.green = 0.0f;
      rgb.blue = c;
      break;
    case 5:
      rgb.red = c;
      rgb.green = 0.0f;
      rgb.blue = x;
      break;
  }
  rgb.red = (rgb.red + m) * 255.0f;
  rgb.green = (rgb.green + m) * 255.0f;
  rgb.blue = (rgb.blue + m) * 255.0f;

  return rgb;
}

/**
 * Convert a RGB color to a HSV color.
 * 
 * @param rgb RGB color
 * @return    HSV color
 * 
 * @link https://www.rapidtables.com/convert/color/rgb-to-hsv.html
 */
hsv_t rgbToHsv(rgb_t rgb) {
  hsv_t hsv;
  /* Change range of RGB values from 0..255 to 0..1 */
  float r = rgb.red / 255.0f;
  float g = rgb.green / 255.0f;
  float b = rgb.blue / 255.0f;
  /* Calculate min, max and delta brightnesses of RGB color components */
  float c_max = fmax(r, fmax(g, b));
  float c_min = fmin(r, fmin(g, b));
  float d = c_max - c_min;
  /* Calculate hue */
  if (d == 0.0f) {
    hsv.hue = 0.0f;
  } else if (c_max == r) {
    hsv.hue = 60.0f * fmod((g - b) / d, 6.0f);
  } else if (c_max == g) {
    hsv.hue = 60.0f * (((b - r) / d) + 2.0f);
  } else if (c_max == b) {
    hsv.hue = 60.0f * (((r - g) / d) + 4.0f);
  }
  if (hsv.hue < 0.0f) {
    hsv.hue += 360.0f;
  }
  /* Calculate saturation */
  if (c_max == 0.0f) {
    hsv.saturation = 0.0f;
  } else {
    hsv.saturation = (d / c_max) * 100.0f;
  }
  /* Calculate value */
  hsv.value = c_max * 100.0f;

  return hsv;
}

/**
 * Reset the custom colors.
 */
void resetDiyColors() {
  const hsv_t resetColor = {.hue = 0.0f, .saturation = 0.0f, .value = 0.0f};
  for (int index = 0; index < (sizeof(diyColors) / sizeof(hsv_t)); index++) {
    diyColors[index] = resetColor;
  }
  writeDiyColorsToEeprom();
}

/**
 * Write the custom colors to EEPROM.
 */
void writeDiyColorsToEeprom() {
  EEPROM.put(EEPROM_STARTING_ADDRESS, diyColors);
  EEPROM.commit();
}
