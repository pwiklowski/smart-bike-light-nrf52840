#include <stdio.h>
#include "light.h"
#include "main.h"
#include "nrf_log.h"
#include "led.h"
#include "service_light.h"

extern AppData app_data;

#define LIGHT_TAG "LIGHT"
#define LIGHT_SETTINGS_KEY "SETTINGS"


void light_settings_load() {
//  esp_err_t err;
//  nvs_handle_t my_handle;
//  err = nvs_open("storage", NVS_READWRITE, &my_handle);
//  if (err != ESP_OK) {
//    ESP_LOGE(LIGHT_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//  } else {
//    size_t len = 100;
//    err = nvs_get_blob(my_handle, LIGHT_SETTINGS_KEY, &app_data, &len);
//    if (err != ESP_OK) {
//      ESP_LOGE(LIGHT_TAG, "Failed to load settings %d", err);
//    }
//    nvs_close(my_handle);
//  }
}

void light_settings_save() {
//  esp_err_t err;
//  nvs_handle_t my_handle;
//  err = nvs_open("storage", NVS_READWRITE, &my_handle);
//  if (err != ESP_OK) {
//    ESP_LOGE(LIGHT_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//  } else {
//    err = nvs_set_blob(my_handle, LIGHT_SETTINGS_KEY, &app_data, sizeof(AppData));
//    if (err != ESP_OK) {
//      ESP_LOGE(LIGHT_TAG, "Failed to save settings %d", err);
//    }
//
//    err = nvs_commit(my_handle);
//    if (err != ESP_OK) {
//      ESP_LOGE(LIGHT_TAG, "Failed to save settings %d", err);
//    }
//    nvs_close(my_handle);
//  }
}

void light_init() {
  led_init();

  light_settings_load();

  app_data.front_params.length = 10;
  app_data.front_params.animation_task = NULL;
  //app_data.back_params.length = &led_strip_back;
  app_data.back_params.animation_task = NULL;

  //start in OFF mode
  app_data.front_params.toggle = 0;
  app_data.back_params.toggle = 0;


  app_data.front_params.power = 100;
  app_data.front_params.red = 0;
  app_data.front_params.green = 0;
  app_data.front_params.blue = 100;


  animation_start(SNAKE, &app_data.front_params);
  //animation_start(OFF, &app_data.back_params);
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

