/*
 * bsp.c
 *
 *  Created on: Mar 31, 2021
 *      Author: pwiklowski
 */
#include "bspconfig.h"
#include "nrf_log.h"
#include "service_light.h"
#include "FreeRTOS.h"
#include "task.h"

uint8_t value;
uint8_t on[] = { 1 };
uint8_t off[] = { 0 };
extern service_light_t m_light_service;

void toggle() {
  uint8_t front = service_light_get_toggle_front();
  uint8_t back = service_light_get_toggle_back();

  NRF_LOG_INFO("bsp_event_handler %d %d", front, back);

  if (front == 0 || back == 0) {
    service_light_update(CHAR_UUID_FRONT_LIGHT_TOGGLE, on, true);
    service_light_update(CHAR_UUID_BACK_LIGHT_TOGGLE, on, true);
    bsp_board_led_on(BSP_BOARD_LED_2);
  } else {
    service_light_update(CHAR_UUID_FRONT_LIGHT_TOGGLE, off, true);
    service_light_update(CHAR_UUID_BACK_LIGHT_TOGGLE, off, true);
    bsp_board_led_off(BSP_BOARD_LED_2);
  }

  vTaskDelete(NULL);
}

void bsp_event_handler(bsp_event_t event) {

  switch (event) {
    case BSP_EVENT_KEY_0:
      xTaskCreate(toggle, "toggle", 64, NULL, configMAX_PRIORITIES - 1, NULL);
      break;
    default:
      break;
  }
}

void buttons_leds_init() {
  ret_code_t err_code;
  bsp_event_t startup_event;

  err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
  APP_ERROR_CHECK(err_code);

  err_code = bsp_btn_ble_init(NULL, &startup_event);
  APP_ERROR_CHECK(err_code);
}
