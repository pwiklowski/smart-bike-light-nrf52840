/*
 * service_light.h
 *
 *  Created on: Apr 2, 2021
 *      Author: pwiklowski
 */

#ifndef SERVICE_LIGHT_H_
#define SERVICE_LIGHT_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"

#define SERVICE_LIGHT_BLE_OBSERVER_PRIO   2
#define SERVICE_LIGHT_UUID_BASE { 0x2c, 0x7f, 0x29, 0x37, 0x52, 0x12, 0x48, 0xfe, 0xa0, 0xe2, 0xa0, 0x75, 0x5b, 0xb6, 0xc9, 0x10 }

#define SERVICE_LIGHT_UUID   0x6900
#define COMMAND_CHAR_UUID    0x6901

typedef struct {
  uint16_t service_handle;
  ble_gatts_char_handles_t command_char_handles;
  uint16_t conn_handle;
  uint8_t uuid_type;
} service_light_t;

void service_light_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);
void service_light_init(void);

#endif /* SERVICE_LIGHT_H_ */
