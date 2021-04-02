/*
 * led.c
 *
 *  Created on: Mar 30, 2021
 *      Author: pwiklowski
 */
#include <stdint.h>
#include "nrf_drv_i2s.h"
#include "sdk_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "nrf_log.h"
#include "nrf_gpio.h"

#define NLEDS 10
#define RESET_BITS 6
#define I2S_BUFFER_SIZE 3*NLEDS + RESET_BITS

uint32_t m_buffer_tx[I2S_BUFFER_SIZE];
static volatile int nled = 1;

#define I2S_SDIN_PIN 26
#define I2S_SDOUT_PIN 27

nrf_drv_i2s_buffers_t const initial_buffers = { .p_tx_buffer = m_buffer_tx };

static void data_handler(nrfx_i2s_buffers_t const *p_released, uint32_t status) {
}

void i2s_init() {
  nrf_drv_i2s_config_t config = NRF_DRV_I2S_DEFAULT_CONFIG;
  config.sdin_pin = NRF_GPIO_PIN_MAP(0, 26);
  config.sdout_pin = NRF_GPIO_PIN_MAP(0, 27);
  config.mck_setup = NRF_I2S_MCK_32MDIV15; ///< 32 MHz / 10 = 3.2 MHz.
  config.ratio = NRF_I2S_RATIO_32X;    ///< LRCK = MCK / 32.
  config.channels = NRF_I2S_CHANNELS_STEREO;

  ret_code_t err_code = nrf_drv_i2s_init(&config, data_handler);
  APP_ERROR_CHECK(err_code);
}

uint32_t convert_color_value(uint8_t level) {
  uint32_t val = 0;

  // 0
  if (level == 0) {
    val = 0x88888888;
  } else if (level == 255) {
    val = 0xeeeeeeee;
  } else {
    val = 0x88888888;
    for (uint8_t i = 0; i < 8; i++) {
      if ((1 << i) & level) {
        uint32_t mask = ~(0x0f << 4 * i);
        uint32_t patt = (0x0e << 4 * i);
        val = (val & mask) | patt;
      }
    }

    // swap 16 bits
    val = (val >> 16) | (val << 16);
  }

  return val;
}

void set_led_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  NRF_LOG_INFO("set_led_color %d %d %d %d", index, r, g, b);

  m_buffer_tx[index * 3] = convert_color_value(g);
  m_buffer_tx[index * 3 + 1] = convert_color_value(r);
  m_buffer_tx[index * 3 + 2] = convert_color_value(b);
}

void set_led_data(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NLEDS; i ++) {
    set_led_color(i, r, g, b);
  }

  for (int i = 3 * NLEDS; i < I2S_BUFFER_SIZE; i++) {
    m_buffer_tx[i] = 0;
  }
}

void led_init() {
  set_led_data(10, 0, 0);
  i2s_init();
  nrf_drv_i2s_start(&initial_buffers, I2S_BUFFER_SIZE, 0);
}

void led_deinit()  {
  nrf_drv_i2s_stop();
}


