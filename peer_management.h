/*
 * peer_management.h
 *
 *  Created on: Mar 29, 2021
 *      Author: pwiklowski
 */

#ifndef PEER_MANAGEMENT_H_
#define PEER_MANAGEMENT_H_

#include "peer_manager.h"
#include "peer_manager_handler.h"

void peer_manager_init(void);
void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context);
void delete_bonds(void);
void pm_evt_handler(pm_evt_t const *p_evt);

#endif /* PEER_MANAGEMENT_H_ */
