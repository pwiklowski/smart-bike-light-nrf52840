/*
 * ble.h
 *
 *  Created on: Mar 29, 2021
 *      Author: pwiklowski
 */

#ifndef BLE_H_
#define BLE_H_


void ble_stack_init(void);
void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context);



#endif /* BLE_H_ */
