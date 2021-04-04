/*
 * led.h
 *
 *  Created on: Mar 30, 2021
 *      Author: pwiklowski
 */

#ifndef LED_H_
#define LED_H_

#include "nrf_drv_spi.h"

typedef struct {
  const nrf_drv_spi_t* spi;
  uint32_t* buffer;
  uint16_t length;
  uint16_t buffer_length;
} led_strip_t;

led_strip_t led_init(uint8_t spi_number, uint16_t strip_length);
void set_led_color(led_strip_t* led_strip, uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void led_update(led_strip_t* led_strip);

#endif /* LED_H_ */
