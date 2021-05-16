#include <stdio.h>
#include "light.h"
#include "main.h"
#include "nrf_log.h"
#include "led.h"
#include "service_light.h"
#include "storage.h"

#include <stdlib.h>
#include "cJSON/cJSON.h"

extern AppData app_data;

#define LIGHT_TAG "LIGHT"
#define LIGHT_SETTINGS_KEY "SETTINGS"

Animation front_animations[] = {
  SOLID,
  PULSE,
  SNAKE,
  CHRISTMAS,
  CHRISTMAS2,
  STROBE,
  STROBE_FAST,
  STROBE_CENTER,
  STROBE_CENTER_INVERT,
  STROBE_CENTER_2
};
Animation back_animations[] = {
  SOLID,
  PULSE,
  SNAKE,
  CHRISTMAS,
  CHRISTMAS2,
  STROBE,
  STROBE_FAST,
  STROBE_CENTER,
  STROBE_CENTER_INVERT,
  STROBE_CENTER_2
};

led_strip_t led_strip_1;
led_strip_t led_strip_2;

void light_settings_load() {
  storage_read((uint8_t*) &app_data, sizeof(app_data));
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
  storage_write((uint8_t*) &app_data, sizeof(app_data));
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

void light_set_value(uint16_t char_uuid, const uint8_t *data, const uint16_t len) {
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

void light_append_settings_list(cJSON *light, Animation animation_list[]) {
  cJSON *front_animations_list = cJSON_CreateArray();

  for (int i = 0; i < sizeof(front_animations); i++) {
    cJSON *animation = cJSON_CreateString(animation_get_name(animation_list[i]));
    cJSON_AddItemToArray(front_animations_list, animation);
  }
  cJSON_AddItemToObject(light, "settings", front_animations_list);
}

cJSON* light_get_lights() {
  cJSON *lights = cJSON_CreateArray();

  cJSON *front = cJSON_CreateObject();
  cJSON_AddStringToObject(front, "name", "front");
  cJSON_AddStringToObject(front, "setting", animation_get_name(app_data.front_params.mode));
  cJSON_AddItemToArray(lights, front);
  light_append_settings_list(front, front_animations);

  cJSON *back = cJSON_CreateObject();
  cJSON_AddStringToObject(back, "name", "back");
  cJSON_AddStringToObject(back, "setting", animation_get_name(app_data.back_params.mode));
  cJSON_AddItemToArray(lights, back);
  light_append_settings_list(back, back_animations);

  return lights;
}

void light_handle_message(uint8_t *message, uint16_t len) {
  uint16_t messageId = message[0] << 8 | message[1];

  NRF_LOG_INFO("request %s", (char* )&message[2]);

  if (strcmp("/lights", (char*) &message[2]) == 0) {
    cJSON *lights = light_get_lights();

    char *string = cJSON_PrintUnformatted(lights);

    service_light_send_response(messageId, (uint8_t*) string, strlen(string));

    cJSON_Delete(lights);
    free(string);
  } else {
    service_light_send_response(messageId, (uint8_t*) "404", 3);
  }
}

