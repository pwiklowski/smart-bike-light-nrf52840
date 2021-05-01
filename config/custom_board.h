/*
 * custom_board.h
 *
 *  Created on: Apr 24, 2021
 *      Author: pwiklowski
 */

#ifndef CONFIG_CUSTOM_BOARD_H_
#define CONFIG_CUSTOM_BOARD_H_

#include "nrf_gpio.h"


#define BUTTONS_NUMBER 1

#define BUTTON_1       NRF_GPIO_PIN_MAP(1, 6)
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1 }

#define BSP_BUTTON_0   BUTTON_1


#define LEDS_NUMBER    4

#define LED_1          NRF_GPIO_PIN_MAP(0,13)
#define LED_2          NRF_GPIO_PIN_MAP(0,14)
#define LED_3          NRF_GPIO_PIN_MAP(0,15)
#define LED_4          NRF_GPIO_PIN_MAP(0,16)
#define LED_START      LED_1
#define LED_STOP       LED_4

#define LEDS_ACTIVE_STATE 0

#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }

#define LEDS_INV_MASK  LEDS_MASK



#endif /* CONFIG_CUSTOM_BOARD_H_ */
