/*
 * bsp.h
 *
 *  Created on: Apr 7, 2021
 *      Author: pwiklowski
 */

#ifndef BSPCONFIG_H_
#define BSPCONFIG_H_


#include "bsp_btn_ble.h"

#define BUTTONS_NUMBER 4

#define BUTTON_1       11
#define BUTTON_2       12
#define BUTTON_3       24
#define BUTTON_4       25
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4 }

#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2
#define BSP_BUTTON_2   BUTTON_3
#define BSP_BUTTON_3   BUTTON_4


void buttons_leds_init();


#endif /* BSPCONFIG_H_ */
