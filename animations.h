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
#include "led.h"

typedef enum {
  SOLID,
  PULSE,
  SNAKE,
  CHRISTMAS,
  CHRISTMAS2,
  STROBE,
  STROBE_FAST,
  STROBE_CENTER,
  STROBE_CENTER_INVERT,
  STROBE_CENTER_2,

  OFF,
  INVALID
} Animation;

typedef struct {
  led_strip_t* led_strip;

  uint8_t toggle;

  Animation mode;

  uint8_t power;
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  TaskHandle_t animation_task;
} AnimationParameters;


void animation_start(Animation anim, AnimationParameters* params);
const char* animation_get_name(Animation animation);
Animation animation_get_enum(char* animation);


#endif /* MAIN_ANIMATIONS_H_ */
