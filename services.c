/*
 * services.c
 *
 *  Created on: Apr 2, 2021
 *      Author: pwiklowski
 */
#include "service_battery.h"
#include "service_light.h"
#include "app_error.h"
#include "nrf_ble_qwr.h"


void services_init(void) {
  battery_service_init();
  service_light_init();
}




