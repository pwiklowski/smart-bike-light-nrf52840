/*
 * animations.h
 *
 *  Created on: Dec 6, 2020
 *      Author: pwiklowski
 */

#ifndef MAIN_ANIMATIONS_H_
#define MAIN_ANIMATIONS_H_

#include <stdio.h>
#include "animations.h"
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
  SOLID,
  PULSE,
  SNAKE,
  CHRISTMAS,
  CHRISTMAS2,

  OFF
} Animation;

typedef struct {
  uint16_t length;
  uint8_t toggle;

  uint8_t mode;

  uint8_t power;
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  TaskHandle_t animation_task;
} AnimationParameters;


void animation_start(Animation anim, AnimationParameters* params);


#endif /* MAIN_ANIMATIONS_H_ */