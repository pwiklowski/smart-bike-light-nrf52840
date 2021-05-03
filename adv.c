/*
 * adv.c
 *
 *  Created on: Mar 29, 2021
 *      Author: pwiklowski
 */

#include "adv.h"
#include "ble_advdata.h"

#define APP_ADV_INTERVAL                    300                                     /**in units of 0.625 ms. */
#define APP_ADV_DURATION                    10*100

#define APP_BLE_CONN_CFG_TAG                1

BLE_ADVERTISING_DEF(m_advertising); /**< Advertising module instance. */

static ble_uuid_t m_adv_uuids[] = /**< Universally unique service identifiers. */
{
{ BLE_UUID_BATTERY_SERVICE, BLE_UUID_TYPE_BLE },
};

ble_advdata_manuf_data_t adv_manuf_data;
uint8_t adv_manuf_data_data[] = { 0x69 };

void advertising_set_adv_data(ble_advdata_t *advdata) {
  advdata->name_type = BLE_ADVDATA_FULL_NAME;
  advdata->include_appearance = true;
  advdata->flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  advdata->uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  advdata->uuids_complete.p_uuids = m_adv_uuids;

  adv_manuf_data.data.p_data = adv_manuf_data_data;
  adv_manuf_data.data.size = sizeof(adv_manuf_data_data);
  adv_manuf_data.company_identifier = 0x0059; //Nordic's company ID

  advdata->p_manuf_specific_data = &adv_manuf_data;
}

void advertising_init(void) {
  ret_code_t err_code;
  ble_advertising_init_t init;

  memset(&init, 0, sizeof(init));

  advertising_set_adv_data(&init.advdata);

  init.config.ble_adv_fast_enabled = true;
  init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
  init.config.ble_adv_fast_timeout = APP_ADV_DURATION;

  init.config.ble_adv_slow_enabled = true;
  init.config.ble_adv_slow_interval = 1285 * 1.6;
  init.config.ble_adv_slow_timeout = 0;

  init.evt_handler = on_adv_evt;

  err_code = ble_advertising_init(&m_advertising, &init);
  APP_ERROR_CHECK(err_code);

  ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void advertising_update(uint8_t battery_level) {
  adv_manuf_data_data[0] = battery_level;

  ble_advdata_t advdata;
  memset(&advdata, 0, sizeof(advdata));

  advertising_set_adv_data(&advdata);

  ble_advertising_advdata_update(&m_advertising, &advdata, NULL);
}

/**@brief Function for starting advertising. */
void advertising_start(void *p_erase_bonds) {
  bool erase_bonds = *(bool*) p_erase_bonds;

  if (erase_bonds) {
    delete_bonds();
    // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
  } else {
    ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_SLOW);
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
      NRF_LOG_INFO("Fast advertising.") ;
      break;

    case BLE_ADV_EVT_SLOW:
      NRF_LOG_INFO("slow advertising.") ;
      break;

    case BLE_ADV_EVT_IDLE:
      NRF_LOG_INFO("idle advertising.") ;
      break;

    default:
      break;
  }
}

