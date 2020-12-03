#include <FastLED.h>
#include <IRremote.h>

#ifndef BRIGHTNESS_RISE_CODE
#define LED_PIN                       12
#define IR_PIN                        23
#define NUM_OF_LEDS                   65
#define MIN_BRIGHTNESS                1
#define MAX_BRIGHTNESS                25
#define MIN_DELAY_TIME                50
#define MAX_DELAY_TIME                3350
#define BRIGHTNESS_RISE_CODE		      0xFF3AC5
#define BRIGHTNESS_FALL_CODE 		      0xFFBA45
#define ON_OFF_CODE					          0xFF02FD
#define PAUSE_RUN_CODE				        0xFF827D
#define STATIC_RED_CODE				        0xFF1AE5
#define STATIC_ORANGE_CODE			      0xFF2AD5
#define STATIC_DARK_YELLOW_CODE		    0xFF0AF5
#define STATIC_YELLOW_CODE			      0xFF38C7
#define STATIC_LIGHT_YELLOW_CODE	    0xFF18E7
#define STATIC_GREEN_CODE			        0xFF9A65
#define STATIC_LIGHT_GREEN_CODE		    0xFFAA55
#define STATIC_CYAN_CODE			        0xFF8A75
#define STATIC_LIGHT_BLUE_CODE		    0xFFB847
#define STATIC_SKY_BLUE_CODE 		      0xFF9867
#define STATIC_BLUE_CODE			        0xFFA25D
#define STATIC_DARK_BLUE_CODE		      0xFF926D
#define STATIC_LYONS_BLUE_CODE		    0xFFB24D
#define STATIC_PURPLE_CODE			      0xFF7887
#define STATIC_BROWN_CODE			        0xFF58A7
#define STATIC_WHITE_CODE			        0xFF22DD
#define STATIC_MILK_WHITE_CODE		    0xFF12ED
#define STATIC_WHITE_PINK_CODE		    0xFF32CD
#define STATIC_GREEN_WHITE_CODE		    0xFFD827
#define STATIC_BLUE_WHITE			        0xFFF807
#define INCREASE_RED_CODE			        0xFF28D7
#define DECREASE_RED_CODE			        0xFF08F7
#define INCREASE_GREEN_CODE			      0xFFA857
#define DECREASE_GREEN_CODE			      0xFF8877
#define INCREASE_BLUE_CODE			      0xFF6897
#define DECREASE_BLUE_CODE			      0xFF48B7
#define SPEED_UP_CODE				          0xFFE817
#define SPEED_DOWN_CODE				        0xFFC837
#define DIY_KEY_1_CODE				        0xFF30CF
#define DIY_KEY_2_CODE				        0xFFB04F
#define DIY_KEY_3_CODE				        0xFF708F
#define DIY_KEY_4_CODE				        0xFF10EF
#define DIY_KEY_5_CODE				        0xFF906F
#define DIY_KEY_6_CODE				        0xFF50AF
#define AUTOMATIC_CHANGE_CODE		      0xFFF00F
#define FLASH_ON_AND_OFF_CODE		      0xFFD02F
#define THREE_COLOR_JUMPY_CHANGE_CODE	0xFF20DF
#define SEVEN_COLOR_JUMPY_CHANGE_CODE	0xFFA05F
#define THREE_COLOR_FADE_CHANGE_CODE	0xFF609F
#define SEVEN_COLOR_FADE_CHANGE_CODE	0xFFE01F
#define HOLD_CODE				              0xFFFFFFFF
#endif

const CRGB BLACK_COLOR = CRGB(0, 0, 0);
const CRGB RED_COLOR = CRGB(255, 0, 0);
const CRGB ORANGE_COLOR = CRGB(255, 64, 0);
const CRGB DARK_YELLOW_COLOR = CRGB(255, 128, 0);
const CRGB YELLOW_COLOR = CRGB(255, 192, 0);
const CRGB LIGHT_YELLOW_COLOR = CRGB(255, 255, 0);
const CRGB GREEN_COLOR = CRGB(0, 255, 0);
const CRGB LIGHT_GREEN_COLOR = CRGB(0, 255, 128);
const CRGB CYAN_COLOR = CRGB(0, 255, 192);
const CRGB LIGHT_BLUE_COLOR = CRGB(0, 192, 255);
const CRGB SKY_BLUE_COLOR = CRGB(0, 128, 255);
const CRGB BLUE_COLOR = CRGB(0, 0, 255);
const CRGB DARK_BLUE_COLOR = CRGB(64, 0, 255);
const CRGB LYONS_BLUE_COLOR = CRGB(128, 0, 255);
const CRGB PURPLE_COLOR = CRGB(192, 0, 255);
const CRGB BROWN_COLOR = CRGB(255, 0, 255);
const CRGB WHITE_COLOR = CRGB(255, 255, 255);
const CRGB MILK_WHITE_COLOR = CRGB(255, 128, 255);
const CRGB WHITE_PINK_COLOR = CRGB(255, 225, 255);
const CRGB GREEN_WHITE_COLOR = CRGB(225, 255, 255);
const CRGB BLUE_WHITE_COLOR = CRGB(128, 255, 255);
CRGB DIY_COLOR_1 = WHITE_COLOR;
CRGB DIY_COLOR_2 = WHITE_COLOR;
CRGB DIY_COLOR_3 = WHITE_COLOR;
CRGB DIY_COLOR_4 = WHITE_COLOR;
CRGB DIY_COLOR_5 = WHITE_COLOR;
CRGB DIY_COLOR_6 = WHITE_COLOR;

CRGB ledStrip[NUM_OF_LEDS];
IRrecv irReceiver(IR_PIN);
decode_results irDecoder;
CRGB currentColor;
CRGB fromFadeColor;
CRGB toFadeColor;
unsigned long currentTime;
unsigned long oldCurrentTime;
unsigned long previousTime;
unsigned long delayTime = 500;
unsigned long currentMode;
int autoModeIndex;
int maxAutoModeIndex;
int ledBrightness = 10;
boolean isRunningAnimation;
boolean isOn;

void setup() {
  //Start the serial monitor
//  Serial.begin(9600);
//  while (!Serial) {  // Wait for the serial connection to be establised.
//    delay(50);
//  }

  //Start the LED service
  FastLED.addLeds<NEOPIXEL, LED_PIN>(ledStrip, NUM_OF_LEDS);

  //Start the IR receiver
  irReceiver.enableIRIn();
}

void loop() { 
  //Get the current time
  oldCurrentTime = currentTime;
  currentTime = millis();
  
  //If a button on the remote was pressed, get the button code
  if (irReceiver.decode(&irDecoder)) {
  	//Ignore a button being held
  	if (irDecoder.value != HOLD_CODE) {
      if (irDecoder.value == ON_OFF_CODE) {
        isOn = !isOn;
        if (isOn) {
          autoModeIndex = 0;
          previousTime = currentTime;
          isRunningAnimation = true;
        }
      }
      if (isOn) {
        switch (irDecoder.value) {
          case STATIC_RED_CODE:
          case STATIC_ORANGE_CODE:
          case STATIC_DARK_YELLOW_CODE:
          case STATIC_YELLOW_CODE:
          case STATIC_LIGHT_YELLOW_CODE:
          case STATIC_GREEN_CODE:
          case STATIC_LIGHT_GREEN_CODE:
          case STATIC_CYAN_CODE:
          case STATIC_LIGHT_BLUE_CODE:
          case STATIC_SKY_BLUE_CODE:
          case STATIC_BLUE_CODE:
          case STATIC_DARK_BLUE_CODE:
          case STATIC_LYONS_BLUE_CODE:
          case STATIC_PURPLE_CODE:
          case STATIC_BROWN_CODE:
          case STATIC_WHITE_CODE:
          case STATIC_MILK_WHITE_CODE:
          case STATIC_WHITE_PINK_CODE:
          case STATIC_GREEN_WHITE_CODE:
          case STATIC_BLUE_WHITE:
          case DIY_KEY_1_CODE:
          case DIY_KEY_2_CODE:
          case DIY_KEY_3_CODE:
          case DIY_KEY_4_CODE:
          case DIY_KEY_5_CODE:
          case DIY_KEY_6_CODE:
            autoModeIndex = 0;
            maxAutoModeIndex = 0;
            currentMode = irDecoder.value;
            previousTime = currentTime;
            isRunningAnimation = false;
            break;
          case FLASH_ON_AND_OFF_CODE:
            autoModeIndex = 0;
            maxAutoModeIndex = 1;
            currentMode = irDecoder.value;
            previousTime = currentTime;
            isRunningAnimation = true;
            break;
          case THREE_COLOR_JUMPY_CHANGE_CODE:
          case THREE_COLOR_FADE_CHANGE_CODE:
            autoModeIndex = 0;
            maxAutoModeIndex = 2;
            currentMode = irDecoder.value;
            previousTime = currentTime;
            isRunningAnimation = true;
            break;
          case SEVEN_COLOR_JUMPY_CHANGE_CODE:
          case SEVEN_COLOR_FADE_CHANGE_CODE:
            autoModeIndex = 0;
            maxAutoModeIndex = 6;
            currentMode = irDecoder.value;
            previousTime = currentTime;
            isRunningAnimation = true;
            break;
          case AUTOMATIC_CHANGE_CODE:
            autoModeIndex = 0;
            maxAutoModeIndex = 23;
            currentMode = irDecoder.value;
            previousTime = currentTime;
            isRunningAnimation = true;
            break;
          case INCREASE_RED_CODE:
          case DECREASE_RED_CODE:
          case INCREASE_GREEN_CODE:
          case DECREASE_GREEN_CODE:
          case INCREASE_BLUE_CODE:
          case DECREASE_BLUE_CODE:
            switch (currentMode) {
              case DIY_KEY_1_CODE:
                DIY_COLOR_1 = adjustDiyColor(DIY_COLOR_1, irDecoder.value);
                break;
              case DIY_KEY_2_CODE:
                DIY_COLOR_2 = adjustDiyColor(DIY_COLOR_2, irDecoder.value);
                break;
              case DIY_KEY_3_CODE:
                DIY_COLOR_3 = adjustDiyColor(DIY_COLOR_3, irDecoder.value);
                break;
              case DIY_KEY_4_CODE:
                DIY_COLOR_4 = adjustDiyColor(DIY_COLOR_4, irDecoder.value);
                break;
              case DIY_KEY_5_CODE:
                DIY_COLOR_5 = adjustDiyColor(DIY_COLOR_5, irDecoder.value);
                break;
              case DIY_KEY_6_CODE:
                DIY_COLOR_6 = adjustDiyColor(DIY_COLOR_6, irDecoder.value);
                break;
            }
            break;
          case BRIGHTNESS_RISE_CODE:
          case BRIGHTNESS_FALL_CODE:
            if (maxAutoModeIndex == 0) {
              switch (irDecoder.value) {
                case BRIGHTNESS_RISE_CODE:
                  ledBrightness = min(ledBrightness + 1, MAX_BRIGHTNESS);
                  break;
                case BRIGHTNESS_FALL_CODE:
                  ledBrightness = max(ledBrightness - 1, MIN_BRIGHTNESS);
                  break;
              }
            }
            break;
          case SPEED_UP_CODE:
          case SPEED_DOWN_CODE:
            if (maxAutoModeIndex > 0) {
              switch (irDecoder.value) {
                case SPEED_UP_CODE:
                  delayTime = max(delayTime - 10, MIN_DELAY_TIME);
                  break;
                case SPEED_DOWN_CODE:
                  delayTime = min(delayTime + 10, MAX_DELAY_TIME);
                  break;
              }
            }
            break;
          case PAUSE_RUN_CODE:
            if (maxAutoModeIndex > 0) {
              isRunningAnimation = !isRunningAnimation;
            }
            break;
        }
      }
  	}
    irReceiver.resume();
  }

  //Set the color of the LEDs
  if (!isRunningAnimation || !isOn) {
    previousTime += (currentTime - oldCurrentTime);
  }
  if ((currentTime - previousTime) > delayTime) {
    if (autoModeIndex == maxAutoModeIndex) {
      autoModeIndex = 0;
    } else {
      ++autoModeIndex;
    }
    previousTime = currentTime;
  }
  switch (currentMode) {
    case STATIC_RED_CODE:
      fromFadeColor = RED_COLOR;
      toFadeColor = RED_COLOR;
      break;
    case STATIC_ORANGE_CODE:
      fromFadeColor = ORANGE_COLOR;
      toFadeColor = ORANGE_COLOR;
      break;
    case STATIC_DARK_YELLOW_CODE:
      fromFadeColor = DARK_YELLOW_COLOR;
      toFadeColor = DARK_YELLOW_COLOR;
      break;
    case STATIC_YELLOW_CODE:
      fromFadeColor = YELLOW_COLOR;
      toFadeColor = YELLOW_COLOR;
      break;
    case STATIC_LIGHT_YELLOW_CODE:
      fromFadeColor = LIGHT_YELLOW_COLOR;
      toFadeColor = LIGHT_YELLOW_COLOR;
      break;
    case STATIC_GREEN_CODE:
      fromFadeColor = GREEN_COLOR;
      toFadeColor = GREEN_COLOR;
      break;
    case STATIC_LIGHT_GREEN_CODE:
      fromFadeColor = LIGHT_GREEN_COLOR;
      toFadeColor = LIGHT_GREEN_COLOR;
      break;
    case STATIC_CYAN_CODE:
      fromFadeColor = CYAN_COLOR;
      toFadeColor = CYAN_COLOR;
      break;
    case STATIC_LIGHT_BLUE_CODE:
      fromFadeColor = LIGHT_BLUE_COLOR;
      toFadeColor = LIGHT_BLUE_COLOR;
      break;
    case STATIC_SKY_BLUE_CODE:
      fromFadeColor = SKY_BLUE_COLOR;
      toFadeColor = SKY_BLUE_COLOR;
      break;
    case STATIC_BLUE_CODE:
      fromFadeColor = BLUE_COLOR;
      toFadeColor = BLUE_COLOR;
      break;
    case STATIC_DARK_BLUE_CODE:
      fromFadeColor = DARK_BLUE_COLOR;
      toFadeColor = DARK_BLUE_COLOR;
      break;
    case STATIC_LYONS_BLUE_CODE:
      fromFadeColor = LYONS_BLUE_COLOR;
      toFadeColor = LYONS_BLUE_COLOR;
      break;
    case STATIC_PURPLE_CODE:
      fromFadeColor = PURPLE_COLOR;
      toFadeColor = PURPLE_COLOR;
      break;
    case STATIC_BROWN_CODE:
      fromFadeColor = BROWN_COLOR;
      toFadeColor = BROWN_COLOR;
      break;
    case STATIC_WHITE_CODE:
      fromFadeColor = WHITE_COLOR;
      toFadeColor = WHITE_COLOR;
      break;
    case STATIC_MILK_WHITE_CODE:
      fromFadeColor = MILK_WHITE_COLOR;
      toFadeColor = MILK_WHITE_COLOR;
      break;
    case STATIC_WHITE_PINK_CODE:
      fromFadeColor = WHITE_PINK_COLOR;
      toFadeColor = WHITE_PINK_COLOR;
      break;
    case STATIC_GREEN_WHITE_CODE:
      fromFadeColor = GREEN_WHITE_COLOR;
      toFadeColor = GREEN_WHITE_COLOR;
      break;
    case STATIC_BLUE_WHITE:
      fromFadeColor = BLUE_WHITE_COLOR;
      toFadeColor = BLUE_WHITE_COLOR;
      break;
    case DIY_KEY_1_CODE:
      fromFadeColor = DIY_COLOR_1;
      toFadeColor = DIY_COLOR_1;
      break;
    case DIY_KEY_2_CODE:
      fromFadeColor = DIY_COLOR_2;
      toFadeColor = DIY_COLOR_2;
      break;
    case DIY_KEY_3_CODE:
      fromFadeColor = DIY_COLOR_3;
      toFadeColor = DIY_COLOR_3;
      break;
    case DIY_KEY_4_CODE:
      fromFadeColor = DIY_COLOR_4;
      toFadeColor = DIY_COLOR_4;
      break;
    case DIY_KEY_5_CODE:
      fromFadeColor = DIY_COLOR_5;
      toFadeColor = DIY_COLOR_5;
      break;
    case DIY_KEY_6_CODE:
      fromFadeColor = DIY_COLOR_6;
      toFadeColor = DIY_COLOR_6;
      break;
    case FLASH_ON_AND_OFF_CODE:
      switch (autoModeIndex) {
        case 0:
          fromFadeColor = BLACK_COLOR;
          toFadeColor = BLACK_COLOR;
          break;
        case 1:
          fromFadeColor = WHITE_COLOR;
          toFadeColor = WHITE_COLOR;
          break;
      }
      break;
    case THREE_COLOR_JUMPY_CHANGE_CODE:
    case SEVEN_COLOR_JUMPY_CHANGE_CODE:
      switch (autoModeIndex) {
        case 0:
          fromFadeColor = RED_COLOR;
          toFadeColor = RED_COLOR;
          break;
        case 1:
          fromFadeColor = GREEN_COLOR;
          toFadeColor = GREEN_COLOR;
          break;
        case 2:
          fromFadeColor = BLUE_COLOR;
          toFadeColor = BLUE_COLOR;
          break;
        case 3:
          fromFadeColor = LIGHT_YELLOW_COLOR;
          toFadeColor = LIGHT_YELLOW_COLOR;
          break;
        case 4:
          fromFadeColor = BROWN_COLOR;
          toFadeColor = BROWN_COLOR;
          break;
        case 5:
          fromFadeColor = CYAN_COLOR;
          toFadeColor = CYAN_COLOR;
          break;
        case 6:
          fromFadeColor = WHITE_COLOR;
          toFadeColor = WHITE_COLOR;
          break;
      }
      break;
    case THREE_COLOR_FADE_CHANGE_CODE:
    case SEVEN_COLOR_FADE_CHANGE_CODE:
      switch (autoModeIndex) {
        case 0:
          fromFadeColor = RED_COLOR;
          toFadeColor = GREEN_COLOR;
          break;
        case 1:
          fromFadeColor = GREEN_COLOR;
          toFadeColor = BLUE_COLOR;
          break;
        case 2:
          fromFadeColor = BLUE_COLOR;
          toFadeColor = (currentMode == THREE_COLOR_FADE_CHANGE_CODE) ? RED_COLOR : LIGHT_YELLOW_COLOR;
          break;
        case 3:
          fromFadeColor = LIGHT_YELLOW_COLOR;
          toFadeColor = BROWN_COLOR;
          break;
        case 4:
          fromFadeColor = BROWN_COLOR;
          toFadeColor = CYAN_COLOR;
          break;
        case 5:
          fromFadeColor = CYAN_COLOR;
          toFadeColor = WHITE_COLOR;
          break;
        case 6:
          fromFadeColor = WHITE_COLOR;
          toFadeColor = RED_COLOR;
          break;
      }
      break;
    case AUTOMATIC_CHANGE_CODE:
      switch (autoModeIndex) {
        case 0:
        case 3:
          fromFadeColor = RED_COLOR;
          toFadeColor = RED_COLOR;
          break;
        case 1:
        case 4:
          fromFadeColor = GREEN_COLOR;
          toFadeColor = GREEN_COLOR;
          break;
        case 2:
        case 5:
          fromFadeColor = BLUE_COLOR;
          toFadeColor = BLUE_COLOR;
          break;
        case 6:
          fromFadeColor = LIGHT_YELLOW_COLOR;
          toFadeColor = LIGHT_YELLOW_COLOR;
          break;
        case 7:
          fromFadeColor = BROWN_COLOR;
          toFadeColor = BROWN_COLOR;
          break;
        case 8:
          fromFadeColor = CYAN_COLOR;
          toFadeColor = CYAN_COLOR;
          break;
        case 9:
          fromFadeColor = WHITE_COLOR;
          toFadeColor = WHITE_COLOR;
          break;
        case 10:
        case 17:
          fromFadeColor = RED_COLOR;
          toFadeColor = GREEN_COLOR;
          break;
        case 11:
        case 18:
          fromFadeColor = GREEN_COLOR;
          toFadeColor = BLUE_COLOR;
          break;
        case 12:
          fromFadeColor = BLUE_COLOR;
          toFadeColor = LIGHT_YELLOW_COLOR;
          break;
        case 13:
          fromFadeColor = LIGHT_YELLOW_COLOR;
          toFadeColor = CYAN_COLOR;
          break;
        case 14:
          fromFadeColor = CYAN_COLOR;
          toFadeColor = BROWN_COLOR;
          break;
        case 15:
          fromFadeColor = BROWN_COLOR;
          toFadeColor = WHITE_COLOR;
          break;
        case 16:
          fromFadeColor = WHITE_COLOR;
          toFadeColor = RED_COLOR;
          break;
        case 19:
          fromFadeColor = BLUE_COLOR;
          toFadeColor = RED_COLOR;
          break;
        case 20:
        case 22:
          fromFadeColor = WHITE_COLOR;
          toFadeColor = WHITE_COLOR;
          break;
        case 21:
        case 23:
          fromFadeColor = BLACK_COLOR;
          toFadeColor = BLACK_COLOR;
          break;
      }
      break;
  }
  currentColor.r = map(ledBrightness, 0, MAX_BRIGHTNESS, 0, map(currentTime - previousTime, 0, delayTime, isOn ? fromFadeColor.r : 0, isOn ? toFadeColor.r : 0));
  currentColor.g = map(ledBrightness, 0, MAX_BRIGHTNESS, 0, map(currentTime - previousTime, 0, delayTime, isOn ? fromFadeColor.g : 0, isOn ? toFadeColor.g : 0));
  currentColor.b = map(ledBrightness, 0, MAX_BRIGHTNESS, 0, map(currentTime - previousTime, 0, delayTime, isOn ? fromFadeColor.b : 0, isOn ? toFadeColor.b : 0));
  for (int ledIndex = 0; ledIndex < NUM_OF_LEDS; ledIndex++) {
    ledStrip[ledIndex] = currentColor;
  }
  FastLED.show();
}

CRGB adjustDiyColor(CRGB color, unsigned long buttonCode) {
  switch (buttonCode) {
    case INCREASE_RED_CODE:
      color.r = min(color.r + 4, 255);
      break;
    case DECREASE_RED_CODE:
      color.r = max(color.r - 4, 0);
      break;
    case INCREASE_GREEN_CODE:
      color.g = min(color.g + 4, 255);
      break;
    case DECREASE_GREEN_CODE:
      color.g = max(color.g - 4, 0);
      break;
    case INCREASE_BLUE_CODE:
      color.b = min(color.b + 4, 255);
      break;
    case DECREASE_BLUE_CODE:
      color.b = max(color.b - 4, 0);
      break;
  }
  return color;
}
