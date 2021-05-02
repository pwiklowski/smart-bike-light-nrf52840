/*
 * service_battery.h
 *
 *  Created on: Mar 29, 2021
 *      Author: pwiklowski
 */

#ifndef SERVICE_BATTERY_H_
#define SERVICE_BATTERY_H_

#include <stdint.h>


void battery_service_init();
void battery_level_update(uint8_t battery_level);


#endif /* SERVICE_BATTERY_H_ */
