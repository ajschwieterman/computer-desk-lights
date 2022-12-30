#include <homekit/homekit.h>
#include <homekit/characteristics.h>

homekit_characteristic_t homekitOnOffCharacteristic = HOMEKIT_CHARACTERISTIC_(ON, false);
homekit_characteristic_t homekitBrightnessCharacteristic = HOMEKIT_CHARACTERISTIC_(BRIGHTNESS, 50);
homekit_characteristic_t homekitSaturationCharacteristic = HOMEKIT_CHARACTERISTIC_(SATURATION, (float)0);
homekit_characteristic_t homekitHueCharacteristic = HOMEKIT_CHARACTERISTIC_(HUE, (float)180);

homekit_accessory_t * homekitAccessories[] = {
  HOMEKIT_ACCESSORY(.id = 1, .category = homekit_accessory_category_lightbulb, .services = (homekit_service_t*[]) {
    HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics = (homekit_characteristic_t*[]) {
      HOMEKIT_CHARACTERISTIC(NAME, "Desk Lights"),
      HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Arduino HomeKit"),
      HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
      HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266"),
      HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
      HOMEKIT_CHARACTERISTIC(IDENTIFY, NULL),
      NULL
    }),
    HOMEKIT_SERVICE(LIGHTBULB, .primary = true, .characteristics = (homekit_characteristic_t*[]) {
      &homekitOnOffCharacteristic,
      &homekitBrightnessCharacteristic,
      &homekitSaturationCharacteristic,
      &homekitHueCharacteristic,
      HOMEKIT_CHARACTERISTIC(NAME, "Lights"),
      NULL
    }),
    NULL
  }),
  NULL
};

homekit_server_config_t homekitConfiguration = {
  .accessories = homekitAccessories,
  .password = "111-11-111"
};
