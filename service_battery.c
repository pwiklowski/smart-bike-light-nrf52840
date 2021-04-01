/*
 * service_battery.c
 *
 *  Created on: Mar 29, 2021
 *      Author: pwiklowski
 */

#include "ble_bas.h"
#include "nrf_ble_qwr.h"
#include "sensorsim.h"
#include "app_error.h"

BLE_BAS_DEF( m_bas); /**< Battery service instance. */

sensorsim_cfg_t m_battery_sim_cfg; /**< Battery Level sensor simulator configuration. */

extern nrf_ble_qwr_t m_qwr;

/**@brief Function for performing battery measurement and updating the Battery Level characteristic
 *        in Battery Service.
 */
void battery_level_update(void) {
  ret_code_t err_code;
  uint8_t battery_level = 69;

  err_code = ble_bas_battery_level_update(&m_bas, battery_level, BLE_CONN_HANDLE_ALL);
  if ((err_code != NRF_SUCCESS) &&
      (err_code != NRF_ERROR_INVALID_STATE) &&
      (err_code != NRF_ERROR_RESOURCES) &&
      (err_code != NRF_ERROR_BUSY) &&
      (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)) {
    APP_ERROR_HANDLER(err_code);
  }
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void nrf_qwr_error_handler(uint32_t nrf_error)
{
  APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
void services_init(void)
{
  ret_code_t err_code;
  ble_bas_init_t bas_init;
  nrf_ble_qwr_init_t qwr_init = { 0 };

  // Initialize Queued Write Module.
  qwr_init.error_handler = nrf_qwr_error_handler;

  err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
  APP_ERROR_CHECK(err_code);

  // Initialize Battery Service.
  memset(&bas_init, 0, sizeof(bas_init));

  // Here the sec level for the Battery Service can be changed/increased.
  bas_init.bl_rd_sec = SEC_OPEN;
  bas_init.bl_cccd_wr_sec = SEC_OPEN;
  bas_init.bl_report_rd_sec = SEC_OPEN;

  bas_init.evt_handler = NULL;
  bas_init.support_notification = true;
  bas_init.p_report_ref = NULL;
  bas_init.initial_batt_level = 100;

  err_code = ble_bas_init(&m_bas, &bas_init);
  APP_ERROR_CHECK(err_code);
}

