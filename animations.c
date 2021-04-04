#include <string.h>
#include "animations.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "led.h"
#include "nrf_log.h"


void animation_snake(void *arg) {
  AnimationParameters *params = (AnimationParameters*) arg;
  NRF_LOG_INFO("animation_snake %d %d %d %d %d %d", params->led_strip->length, params->power,
      params->red, params->green , params->blue);

  while (1) {
    for (int j = 0; j < params->led_strip->length; j++) {
      for (int i = 0; i < params->led_strip->length; i++) {
        uint8_t r = params->red;
        uint8_t g = params->green;
        uint8_t b = params->blue;

        float p = i == j ? params->power/100.0: 0;

        set_led_color(params->led_strip, i, p * r, p * g, p * b);
      }
      led_update(params->led_strip);
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  }
}

void animation_pulse(void *arg) {
  AnimationParameters *params = (AnimationParameters*) arg;
  uint8_t steps = 50;

  while (1) {
    for (int step = 0; step < steps; step++) {
      for (int i = 0; i < params->led_strip->length; i++) {
        uint8_t r = params->red;
        uint8_t g = params->green;
        uint8_t b = params->blue;

        float p = params->power/100.0 * ((float) step / steps);

        set_led_color(params->led_strip, i, p * r, p * g, p * b);
      }
      led_update(params->led_strip);

      vTaskDelay(30 / portTICK_PERIOD_MS);
    }
    for (int step = steps; step >= 0; step--) {
      for (int i = 0; i < params->led_strip->length; i++) {
        uint8_t r = params->red;
        uint8_t g = params->green;
        uint8_t b = params->blue;

        float p = params->power/100.0 * ((float) step / steps);

        set_led_color(params->led_strip, i, p * r, p * g, p * b);
      }
      led_update(params->led_strip);

      vTaskDelay(30 / portTICK_PERIOD_MS);
    }
  }
}

void animation_christmas(void *arg) {
  AnimationParameters *params = (AnimationParameters*) arg;
  while (1) {
    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < params->led_strip->length; i++) {
        uint8_t r = (i + j) % 3 == 0 ? 255 : 0;
        uint8_t g = (i + j) % 3 == 1 ? 255 : 0;
        uint8_t b = (i + j) % 3 == 2 ? 255 : 0;

        float p = params->power/100.0;

        set_led_color(params->led_strip, i, p * r, p * g, p * b);
      }
      led_update(params->led_strip);

      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
  }
}

void animation_christmas_2(void *arg) {
  AnimationParameters *params = (AnimationParameters*) arg;

  while (1) {
    for (int j = 0; j < 5; j++) {
      for (int i = 0; i < params->led_strip->length; i++) {
        uint8_t r = (i + j) % 5 == 0 ? 255 : 0;
        uint8_t g = (i + j) % 5 == 0 ? 0 : 255;
        uint8_t b = 0;

        float p = params->power/100.0;

        set_led_color(params->led_strip, i, p * r, p * g, p * b);
      }
      led_update(params->led_strip);

      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
  }
}

void animation_set_solid_color(led_strip_t* led_strip, float power, uint8_t red, uint8_t green, uint8_t blue) {
  for (int i = 0; i < led_strip->length; i++) {
    set_led_color(led_strip, i, power * red, power * green, power * blue);
  }
  led_update(led_strip);

}

void animation_start(Animation anim, AnimationParameters* params) {
  NRF_LOG_INFO("animation_start %d %d %d %d %d %d",anim, params->mode, params->power, params->red, params->green , params->blue);

  if (params->animation_task != NULL) {
    vTaskDelete(params->animation_task);
    params->animation_task = NULL;
  }

  switch (anim) {
  case SOLID:
    animation_set_solid_color(params->led_strip, params->power/100.0, params->red, params->green, params->blue);
    break;
  case SNAKE:
    xTaskCreate(animation_snake, "animation_snake", 512, params,  configMAX_PRIORITIES - 1, &params->animation_task);
    break;
  case PULSE:
    xTaskCreate(animation_pulse, "animation_pulse", 512, params, configMAX_PRIORITIES - 1, &params->animation_task);
    break;
  case CHRISTMAS:
    xTaskCreate(animation_christmas, "animation_christmas", 512, params, configMAX_PRIORITIES - 1, &params->animation_task);
    break;
  case CHRISTMAS2:
    xTaskCreate(animation_christmas_2, "animation_christmas_2", 512, params, configMAX_PRIORITIES - 1, &params->animation_task);
    break;
  default:
    animation_set_solid_color(params->led_strip, 0, 0, 0, 0);
    break;
  }
}
