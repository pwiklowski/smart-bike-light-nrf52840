#include <stdio.h>
#include "light.h"
#include "main.h"
#include "nrf_log.h"
#include "led.h"
#include "service_light.h"
#include "storage.h"

extern AppData app_data;

#define LIGHT_TAG "LIGHT"
#define LIGHT_SETTINGS_KEY "SETTINGS"


led_strip_t led_strip_1;
led_strip_t led_strip_2;

void light_settings_load() {
  storage_read((uint8_t*)&app_data, sizeof(app_data));
}

void light_settings_save() {
  NRF_LOG_INFO("save animation_front %d %d %d %d %d %d",
      app_data.front_params.mode,
      app_data.front_params.power,
      app_data.front_params.red,
      app_data.front_params.green,
      app_data.front_params.blue);

  NRF_LOG_INFO("save animation_back %d %d %d %d %d %d",
      app_data.back_params.mode,
      app_data.back_params.power,
      app_data.back_params.red,
      app_data.back_params.green,
      app_data.back_params.blue);
  storage_write((uint8_t*)&app_data, sizeof(app_data));
}

void light_init() {
  led_strip_1 = led_init(0, 16);
  led_strip_2 = led_init(2, 16);

  light_settings_load();

  app_data.front_params.led_strip = &led_strip_1;
  app_data.front_params.animation_task = NULL;

  app_data.back_params.led_strip = &led_strip_2;
  app_data.back_params.animation_task = NULL;

  //start in OFF mode
  app_data.front_params.toggle = 0;
  app_data.back_params.toggle = 0;

  NRF_LOG_INFO("animation_front %d %d %d %d %d",
      app_data.front_params.mode,
      app_data.front_params.power,
      app_data.front_params.red,
      app_data.front_params.green,
      app_data.front_params.blue);

  NRF_LOG_INFO("animation_back %d %d %d %d %d",
      app_data.back_params.mode,
      app_data.back_params.power,
      app_data.back_params.red,
      app_data.back_params.green,
      app_data.back_params.blue);

  animation_start(OFF, &app_data.front_params);
  animation_start(OFF, &app_data.back_params);
}

void light_set_value(uint16_t char_uuid, const uint8_t* data, const uint16_t len) {
  NRF_LOG_INFO("light_set_value param=%x value=%d len=%d", char_uuid, data[0], len);

  if (char_uuid == CHAR_UUID_FRONT_LIGHT_TOGGLE) {
    app_data.front_params.toggle = data[0];
    if (data[0] == 0) {
      animation_start(OFF, &app_data.front_params);
    } else {
      animation_start(app_data.front_params.mode, &app_data.front_params);
    }
  }

  if (char_uuid == CHAR_UUID_BACK_LIGHT_TOGGLE) {
    app_data.back_params.toggle = data[0];
    if (data[0] == 0) {
      animation_start(OFF, &app_data.back_params);
    } else {
      animation_start(app_data.back_params.mode, &app_data.back_params);
    }
  }

  if (char_uuid == CHAR_UUID_FRONT_LIGHT_MODE) {
    app_data.front_params.mode = data[0];
    animation_start(app_data.front_params.mode, &app_data.front_params);
  }

  if (char_uuid == CHAR_UUID_BACK_LIGHT_MODE) {
    app_data.back_params.mode = data[0];
    animation_start(app_data.back_params.mode, &app_data.back_params);
  }

  if (char_uuid == CHAR_UUID_FRONT_LIGHT_SETTING) {
    app_data.front_params.power = data[0];
    app_data.front_params.red = data[1];
    app_data.front_params.green = data[2];
    app_data.front_params.blue = data[3];

    animation_start(app_data.front_params.mode, &app_data.front_params);
  }

  if (char_uuid == CHAR_UUID_BACK_LIGHT_SETTING) {
    app_data.back_params.power = data[0];
    app_data.back_params.red = data[1];
    app_data.back_params.green = data[2];
    app_data.back_params.blue = data[3];

    animation_start(app_data.back_params.mode, &app_data.back_params);
  }

  if (char_uuid != CHAR_UUID_FRONT_LIGHT_TOGGLE && char_uuid != CHAR_UUID_BACK_LIGHT_TOGGLE) {
    light_settings_save();
  }

}

