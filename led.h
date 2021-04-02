/*
 * led.h
 *
 *  Created on: Mar 30, 2021
 *      Author: pwiklowski
 */

#ifndef LED_H_
#define LED_H_

void led_init();
void led_deinit();
void led_thread();
void set_led_color(uint16_t index, uint8_t r, uint8_t g, uint8_t b);


#endif /* LED_H_ */
