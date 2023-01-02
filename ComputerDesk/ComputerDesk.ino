#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <Timer.h>

#define EEPROM_STARTING_ADDRESS   1500
#define INACTIVE_INDEX            -1
#define IR_RECEIVER_PIN           12
#define NEOPIXEL_COUNT            65
#define NEOPIXEL_PIN              14
#define AUTOMATIC_CHANGE_CODE     0xFFF00F
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
#define FADE_SEVEN_CODE           0xFFE01F
#define FADE_THREE_CODE           0xFF609F
#define FLASH_ON_AND_OFF_CODE     0xFFD02F
#define GREEN_DECREASE_CODE       0xFF8877
#define GREEN_INCREASE_CODE       0xFFA857
#define JUMP_SEVEN_CODE           0xFFA05F
#define JUMP_THREE_CODE           0xFF20DF
#define ON_OFF_CODE               0xFF02FD
#define PAUSE_RUN_CODE            0xFF827D
#define RED_DECREASE_CODE         0xFF08F7
#define RED_INCREASE_CODE         0xFF28D7
#define SPEED_DOWN_CODE           0xFFC837
#define SPEED_UP_CODE             0xFFE817
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

/* Define structure for color transitions */
typedef struct {
  hsv_t color;
  bool fade;
} transistion_t;

/* Define hardware */
IRrecv irReceiver(IR_RECEIVER_PIN);
Adafruit_NeoPixel neopixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

/* Define HomeKit configuration */
extern "C" homekit_server_config_t homekitConfiguration;
extern "C" homekit_characteristic_t homekitOnOffCharacteristic;
extern "C" homekit_characteristic_t homekitBrightnessCharacteristic;
extern "C" homekit_characteristic_t homekitSaturationCharacteristic;
extern "C" homekit_characteristic_t homekitHueCharacteristic;

/* Define colors */
const hsv_t BLACK = {.hue = 0.0f, .saturation = 0.0f, .value = 0.0f};
const hsv_t BLUE = {.hue = 240.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t BLUE_WHITE = {.hue = 300.0f, .saturation = 50.0f, .value = 100.0f};
const hsv_t BROWN = {.hue = 300.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t CYAN = {.hue = 165.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t DARK_BLUE = {.hue = 255.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t DARK_YELLOW = {.hue = 30.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t GREEN = {.hue = 120.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t GREEN_WHITE = {.hue = 180.0f, .saturation = 12.5f, .value = 100.0f};
const hsv_t LIGHT_BLUE = {.hue = 195.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t LIGHT_GREEN = {.hue = 150.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t LIGHT_YELLOW = {.hue = 60.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t LYONS_BLUE = {.hue = 270.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t MILK_WHITE = {.hue = 300.0f, .saturation = 50.0f, .value = 100.0f};
const hsv_t ORANGE = {.hue = 15.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t PURPLE = {.hue = 285.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t RED = {.hue = 0.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t SKY_BLUE = {.hue = 210.0f, .saturation = 100.0f, .value = 100.0f};
const hsv_t WHITE = {.hue = 0.0f, .saturation = 0.0f, .value = 100.0f};
const hsv_t WHITE_PINK = {.hue = 300.0f, .saturation = 12.5f, .value = 100.0f};
const hsv_t YELLOW = {.hue = 45.0f, .saturation = 100.0f, .value = 100.0f};

/* Define animation color sequences */
const transistion_t AUTO_COLORS[25] = {
  {.color = RED, .fade = false},
  {.color = GREEN, .fade = false},
  {.color = BLUE, .fade = false},
  {.color = RED, .fade = false},
  {.color = GREEN, .fade = false},
  {.color = BLUE, .fade = false},
  {.color = LIGHT_YELLOW, .fade = false},
  {.color = BROWN, .fade = false},
  {.color = CYAN, .fade = false},
  {.color = WHITE, .fade = false},
  {.color = RED, .fade = true},
  {.color = GREEN, .fade = true},
  {.color = BLUE, .fade = true},
  {.color = LIGHT_YELLOW, .fade = true},
  {.color = BROWN, .fade = true},
  {.color = CYAN, .fade = true},
  {.color = WHITE, .fade = true},
  {.color = RED, .fade = true},
  {.color = GREEN, .fade = true},
  {.color = BLUE, .fade = true},
  {.color = RED, .fade = true},
  {.color = WHITE, .fade = false},
  {.color = BLACK, .fade = false},
  {.color = WHITE, .fade = false},
  {.color = BLACK, .fade = false},
};
const transistion_t FADE_SEVEN_COLORS[7] = {
  {.color = RED, .fade = true},
  {.color = GREEN, .fade = true},
  {.color = BLUE, .fade = true},
  {.color = LIGHT_YELLOW, .fade = true},
  {.color = BROWN, .fade = true},
  {.color = CYAN, .fade = true},
  {.color = WHITE, .fade = true},
};
const transistion_t FADE_THREE_COLORS[3] = {
  {.color = RED, .fade = true},
  {.color = GREEN, .fade = true},
  {.color = BLUE, .fade = true},
};
const transistion_t FLASH_ON_OFF_COLORS[2] = {
  {.color = BLACK, .fade = false},
  {.color = WHITE, .fade = false},
};
const transistion_t JUMP_SEVEN_COLORS[7] = {
  {.color = RED, .fade = false},
  {.color = GREEN, .fade = false},
  {.color = BLUE, .fade = false},
  {.color = LIGHT_YELLOW, .fade = false},
  {.color = BROWN, .fade = false},
  {.color = CYAN, .fade = false},
  {.color = WHITE, .fade = false},
};
const transistion_t JUMP_THREE_COLORS[3] = {
  {.color = RED, .fade = false},
  {.color = GREEN, .fade = false},
  {.color = BLUE, .fade = false},
};

/* Other variables */
int activeDiyColorIndex = INACTIVE_INDEX;
int activeAnimationIndex = INACTIVE_INDEX;
const transistion_t* animation;
hsv_t diyColors[6];
decode_results irDecoder;
int numberOfAnimationColors;
uint32_t timeIntervalMs = 500;
Timer timer(MILLIS);
bool updateLeds = false;

void setup() {
  /* Start the serial monitor */
  Serial.begin(115200);
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
          case AUTOMATIC_CHANGE_CODE:
            setAnimation(AUTO_COLORS, sizeof(AUTO_COLORS));
            break;
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
          case FADE_SEVEN_CODE:
            setAnimation(FADE_SEVEN_COLORS, sizeof(FADE_SEVEN_COLORS));
            break;
          case FADE_THREE_CODE:
            setAnimation(FADE_THREE_COLORS, sizeof(FADE_THREE_COLORS));
            break;
          case FLASH_ON_AND_OFF_CODE:
            setAnimation(FLASH_ON_OFF_COLORS, sizeof(FLASH_ON_OFF_COLORS));
            break;
          case GREEN_DECREASE_CODE:
            adjustDiyColor(0.0f, -4.0f, 0.0f);
            break;
          case GREEN_INCREASE_CODE:
            adjustDiyColor(0.0f, 4.0f, 0.0f);
            break;
          case JUMP_SEVEN_CODE:
            setAnimation(JUMP_SEVEN_COLORS, sizeof(JUMP_SEVEN_COLORS));
            break;
          case JUMP_THREE_CODE:
            setAnimation(JUMP_THREE_COLORS, sizeof(JUMP_THREE_COLORS));
            break;
          case PAUSE_RUN_CODE:
            setTimerState();
            break;
          case RED_DECREASE_CODE:
            adjustDiyColor(-4.0f, 0.0f, 0.0f);
            break;
          case RED_INCREASE_CODE:
            adjustDiyColor(4.0f, 0.0f, 0.0f);
            break;
          case SPEED_DOWN_CODE:
            setInterval(min(timeIntervalMs + 10, (uint32_t)3000));
            break;
          case SPEED_UP_CODE:
            setInterval(max(timeIntervalMs - 10, (uint32_t)50));
            break;
          case STATIC_BLUE_CODE:
            setStaticColor(BLUE.hue, BLUE.saturation);
            break;
          case STATIC_BLUE_WHITE:
            setStaticColor(BLUE_WHITE.hue, BLUE_WHITE.saturation);
            break;
          case STATIC_BROWN_CODE:
            setStaticColor(BROWN.hue, BROWN.saturation);
            break;
          case STATIC_CYAN_CODE:
            setStaticColor(CYAN.hue, CYAN.saturation);
            break;
          case STATIC_DARK_BLUE_CODE:
            setStaticColor(DARK_BLUE.hue, DARK_BLUE.saturation);
            break;
          case STATIC_DARK_YELLOW_CODE:
            setStaticColor(DARK_YELLOW.hue, DARK_YELLOW.saturation);
            break;
          case STATIC_GREEN_CODE:
            setStaticColor(GREEN.hue, GREEN.saturation);
            break;
          case STATIC_GREEN_WHITE_CODE:
            setStaticColor(GREEN_WHITE.hue, GREEN_WHITE.saturation);
            break;
          case STATIC_LIGHT_BLUE_CODE:
            setStaticColor(LIGHT_BLUE.hue, LIGHT_BLUE.saturation);
            break;
          case STATIC_LIGHT_GREEN_CODE:
            setStaticColor(LIGHT_GREEN.hue, LIGHT_GREEN.saturation);
            break;
          case STATIC_LIGHT_YELLOW_CODE:
            setStaticColor(LIGHT_YELLOW.hue, LIGHT_YELLOW.saturation);
            break;
          case STATIC_LYONS_BLUE_CODE:
            setStaticColor(LYONS_BLUE.hue, LYONS_BLUE.saturation);
            break;
          case STATIC_MILK_WHITE_CODE:
            setStaticColor(MILK_WHITE.hue, MILK_WHITE.saturation);
            break;
          case STATIC_ORANGE_CODE:
            setStaticColor(ORANGE.hue, ORANGE.saturation);
            break;
          case STATIC_PURPLE_CODE:
            setStaticColor(PURPLE.hue, PURPLE.saturation);
            break;
          case STATIC_RED_CODE:
            setStaticColor(RED.hue, RED.saturation);
            break;
          case STATIC_SKY_BLUE_CODE:
            setStaticColor(SKY_BLUE.hue, SKY_BLUE.saturation);
            break;
          case STATIC_WHITE_CODE:
            setStaticColor(WHITE.hue, WHITE.saturation);
            break;
          case STATIC_WHITE_PINK_CODE:
            setStaticColor(WHITE_PINK.hue, WHITE_PINK.saturation);
            break;
          case STATIC_YELLOW_CODE:
            setStaticColor(YELLOW.hue, YELLOW.saturation);
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
  if (isRunningAnimation()) {
    /* Transition to the next color in the animation if the timer expired */
    if (timer.read() >= timeIntervalMs) {
      activeAnimationIndex = (activeAnimationIndex + 1) % numberOfAnimationColors;
      timer.start();
      updateLeds = true;
    }
    /* Always update the LEDs if in a fading transiting */
    if (animation[activeAnimationIndex].fade) {
      updateLeds = true;
    }
  }
  if (updateLeds) {
    uint32_t color = neopixels.Color(0, 0, 0);
    /* Set what the color of the LEDs will be */
    if (homekitOnOffCharacteristic.value.bool_value) {
      rgb_t rgb;
      if (isRunningAnimation()) {
        if (animation[activeAnimationIndex].fade) {
          /* Get the colors to fade from and to */
          hsv_t oldColorHsv = animation[activeAnimationIndex].color;
          hsv_t newColorHsv = animation[(activeAnimationIndex + 1) % numberOfAnimationColors].color;
          /* Adjust the colors to match the current brightness level */
          oldColorHsv.value = (oldColorHsv.value / 100.0f) * homekitBrightnessCharacteristic.value.int_value;
          newColorHsv.value = (newColorHsv.value / 100.0f) * homekitBrightnessCharacteristic.value.int_value;
          /* Perform the fading calculation and create color to display */
          rgb_t oldColorRgb = hsvToRgb(oldColorHsv);
          rgb_t newColorRgb = hsvToRgb(newColorHsv);
          float r = map(timer.read(), 0, timeIntervalMs, oldColorRgb.red, newColorRgb.red);
          float g = map(timer.read(), 0, timeIntervalMs, oldColorRgb.green, newColorRgb.green);
          float b = map(timer.read(), 0, timeIntervalMs, oldColorRgb.blue, newColorRgb.blue);
          rgb = {.red = r, .green = g, .blue = b};
        } else {
          /* Create static color of animation to display */
          rgb = hsvToRgb({
            .hue = animation[activeAnimationIndex].color.hue,
            .saturation = animation[activeAnimationIndex].color.saturation,
            .value = (animation[activeAnimationIndex].color.value / 100.0f) * homekitBrightnessCharacteristic.value.int_value
          });
        }
      } else {
        /* Create static color to display */
        rgb = hsvToRgb({
          .hue = homekitHueCharacteristic.value.float_value,
          .saturation = homekitSaturationCharacteristic.value.float_value,
          .value = homekitBrightnessCharacteristic.value.int_value
        });
      }
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
  if (activeDiyColorIndex != INACTIVE_INDEX) {
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
 * Setup global variables to begin sequencing through an array of colors. This is called when the user presses any
 * of the animation buttons on the IR remote (AUTO, FADE7, FLASH, JUMP3, etc.)
 * 
 * @param a       Pointer to the list of transitions to display
 * @param size    The size of 'animation' in bytes, not in number of transitions
 */
void setAnimation(const transistion_t* a, size_t size) {
  animation = a;
  numberOfAnimationColors = size / sizeof(transistion_t);
  activeAnimationIndex = 0;
  timer.start();
  activeDiyColorIndex = INACTIVE_INDEX;
  updateLeds = true;
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
  activeAnimationIndex = INACTIVE_INDEX;
  activeDiyColorIndex = index;
  timer.stop();
  updateLeds = true;
}

/**
 * Set the amount of time between each animation color is displayed. This is called when the user presses either 
 * of the animation speed buttons on the IR remote (QUICK and SLOW).
 * 
 * @param interval  The new interval time
 */
void setInterval(uint32_t interval) {
  if (isRunningAnimation()) {
    timeIntervalMs = interval;
  }
}

/**
 * Set the on/off state of the LEDs. This is called either when HomeKit receives a request to change the state
 * or the user presses the power button on the IR remote.
 * 
 * @param state   The new on/off state of the LEDs
 */
void setOnOffState(bool state) {
  homekitOnOffCharacteristic.value.bool_value = state;
  if (isRunningAnimation()) {
    activeAnimationIndex = 0;
    timer.start();
  }
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
  activeAnimationIndex = INACTIVE_INDEX;
  activeDiyColorIndex = INACTIVE_INDEX;
  timer.stop();
  updateLeds = true;
}

/**
 * Change the state of the animation timer. This is called when the user presses the PAUSE/RUN button on the IR 
 * remote to pause/continue the current animation running.
 */
void setTimerState() {
  if (isRunningAnimation()) {
    switch (timer.state()) {
      case RUNNING:
        timer.pause();
        break;
      case PAUSED:
        timer.resume();
        break;
    }
  }
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
 * Check if an animation is currently running.
 * 
 * @return True if an animation is running; False otherwise
 */
bool isRunningAnimation() {
  return (activeAnimationIndex != INACTIVE_INDEX);
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
