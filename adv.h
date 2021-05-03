/*
 * adv.h
 *
 *  Created on: Mar 29, 2021
 *      Author: pwiklowski
 */

#ifndef ADV_H_
#define ADV_H_

#include "ble_advdata.h"
#include "ble_advertising.h"
#include "peer_management.h"
#include "ble_bas.h"
#include "nrf_log.h"
#include "app_error.h"


void advertising_init(void);
void advertising_start(void * p_erase_bonds);
void advertising_update(uint8_t battery_level);
void on_adv_evt(ble_adv_evt_t ble_adv_evt);

#endif /* ADV_H_ */
