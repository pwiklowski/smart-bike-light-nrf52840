/*
 * main.h
 *
 *  Created on: Apr 1, 2021
 *      Author: pwiklowski
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "animations.h"

typedef struct {
  AnimationParameters front_params;
  AnimationParameters back_params;

  uint8_t battery_level;
} AppData;



#endif /* MAIN_H_ */
