/*
 * adv.c
 *
 *  Created on: Mar 29, 2021
 *      Author: pwiklowski
 */


#include "adv.h"

#define APP_ADV_INTERVAL                    300                                     /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_DURATION                    18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_BLE_CONN_CFG_TAG                1

BLE_ADVERTISING_DEF( m_advertising); /**< Advertising module instance. */

static ble_uuid_t m_adv_uuids[] = /**< Universally unique service identifiers. */
  {
    { BLE_UUID_BATTERY_SERVICE, BLE_UUID_TYPE_BLE },
  };

/**@brief Function for initializing the Advertising functionality. */
void advertising_init(void) {
  ret_code_t err_code;
  ble_advertising_init_t init;

  memset(&init, 0, sizeof(init));

  init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
  init.advdata.include_appearance = true;
  init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  init.advdata.uuids_complete.p_uuids = m_adv_uuids;

  init.config.ble_adv_fast_enabled = true;
  init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
  init.config.ble_adv_fast_timeout = APP_ADV_DURATION;

  init.evt_handler = on_adv_evt;

  err_code = ble_advertising_init(&m_advertising, &init);
  APP_ERROR_CHECK(err_code);

  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**@brief Function for starting advertising. */
void advertising_start(void *p_erase_bonds) {
  bool erase_bonds = *(bool*) p_erase_bonds;

  if (erase_bonds) {
    delete_bonds();
    // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
  } else {
    ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
  }
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
void on_adv_evt(ble_adv_evt_t ble_adv_evt) {

  switch (ble_adv_evt) {
  case BLE_ADV_EVT_FAST:
    NRF_LOG_INFO("Fast advertising.");
    break;

  case BLE_ADV_EVT_IDLE:
    break;

  default:
    break;
  }
}

