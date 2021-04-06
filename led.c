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
#include "led.h"
#include "stdlib.h"

#define SPI0_SCK_PIN 28
#define SPI0_MOSI_PIN 29

#define SPI2_MOSI_PIN 30
#define SPI2_SCK_PIN 31

#define RESET_LEN 8

const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(0);
const nrf_drv_spi_t spi2 = NRF_DRV_SPI_INSTANCE(2);

uint32_t ntohl(uint32_t const net) {
    uint8_t data[4] = {};
    memcpy(&data, &net, sizeof(data));

    return ((uint32_t) data[3] << 0)
         | ((uint32_t) data[2] << 8)
         | ((uint32_t) data[1] << 16)
         | ((uint32_t) data[0] << 24);
}

uint32_t convert_color_value(uint8_t level) {
  uint32_t val = 0;

  for (uint8_t i = 0; i < 8; i++) {
    if ((1 << i) & level) {
      val = val | (0xe << (i*4));
    } else {
      val = val | (0x8 << (i*4));
    }
  }

  return ntohl(val);
}

void set_led_color(led_strip_t* led_strip, uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  led_strip->buffer[1 + index * 3] = convert_color_value(g);
  led_strip->buffer[1 + index * 3 + 1] = convert_color_value(r);
  led_strip->buffer[1 + index * 3 + 2] = convert_color_value(b);
}

void led_update(led_strip_t* led_strip){
  NRF_LOG_INFO("led update len=%d", led_strip->buffer_length*4);
    APP_ERROR_CHECK(
        nrf_drv_spi_transfer(led_strip->spi, (uint8_t* )led_strip->buffer,
            led_strip->buffer_length * sizeof(uint32_t) , NULL, 0)
    );
}

led_strip_t led_init(uint8_t spi_number, uint16_t strip_length) {
  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.ss_pin   = NRF_DRV_SPI_PIN_NOT_USED;
  spi_config.miso_pin = NRF_DRV_SPI_PIN_NOT_USED;
  spi_config.frequency = NRF_DRV_SPI_FREQ_4M;

  led_strip_t strip;
  strip.length = strip_length;
  strip.buffer_length = strip_length *3 + RESET_LEN + 1;
  strip.buffer = (uint32_t *) calloc(strip.buffer_length, sizeof(uint32_t));

  if (spi_number == 0) {
    strip.spi = &spi0;
    spi_config.mosi_pin = SPI0_MOSI_PIN;
    spi_config.sck_pin  = SPI0_SCK_PIN;
  } else if (spi_number == 2) {
    strip.spi = &spi2;
    spi_config.mosi_pin = SPI2_MOSI_PIN;
    spi_config.sck_pin  = SPI2_SCK_PIN;
  } else {
    APP_ERROR_CHECK(true);
  }

  APP_ERROR_CHECK(nrf_drv_spi_init(strip.spi, &spi_config, NULL, NULL));
  return strip;
}


