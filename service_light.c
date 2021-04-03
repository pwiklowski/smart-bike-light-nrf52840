/*
 * service_light.c
 *
 *  Created on: Apr 2, 2021
 *      Author: pwiklowski
 */

#include <string.h>
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "service_light.h"
#include "nrf_log.h"


service_light_t m_light_service;
NRF_SDH_BLE_OBSERVER(m_light_service_obs, SERVICE_LIGHT_BLE_OBSERVER_PRIO, service_light_on_ble_evt, &m_light_service);


void service_light_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context) {
  NRF_LOG_INFO("service_light_on_ble_evt %d", p_ble_evt->header.evt_id);

  service_light_t *service_light = (service_light_t*) p_context;

  switch (p_ble_evt->header.evt_id) {
  case BLE_GAP_EVT_CONNECTED:
    NRF_LOG_INFO("BLE_GAP_EVT_CONNECTED");
    service_light->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    break;

  case BLE_GAP_EVT_DISCONNECTED:
    NRF_LOG_INFO("BLE_GAP_EVT_DISCONNECTED");
    service_light->conn_handle = BLE_CONN_HANDLE_INVALID;
    break;

  case BLE_GATTS_EVT_WRITE:{
    ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    NRF_LOG_INFO("BLE_GATTS_EVT_WRITE %d", p_evt_write->len);
    break;
  }
  default:
    // No implementation needed.
    break;
  }
}

uint32_t command_char_add(service_light_t *service_light) {

  uint32_t err_code;
  ble_gatts_char_md_t char_md;
  ble_gatts_attr_t attr_char_value;
  ble_uuid_t ble_uuid;
  ble_gatts_attr_md_t attr_md;

  memset(&char_md, 0, sizeof(char_md));

  char_md.char_props.read = 1;
  char_md.char_props.write = 1;
  char_md.char_props.notify = 0;
  char_md.p_char_user_desc = NULL;
  char_md.p_char_pf = NULL;
  char_md.p_user_desc_md = NULL;
  char_md.p_cccd_md = NULL;
  char_md.p_sccd_md = NULL;

  ble_uuid.type = service_light->uuid_type;
  ble_uuid.uuid = COMMAND_CHAR_UUID;

  memset(&attr_md, 0, sizeof(attr_md));

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
  attr_md.vloc = BLE_GATTS_VLOC_STACK;
  attr_md.rd_auth = 0;
  attr_md.wr_auth = 0;
  attr_md.vlen = 0;

  memset(&attr_char_value, 0, sizeof(attr_char_value));

  attr_char_value.p_uuid = &ble_uuid;
  attr_char_value.p_attr_md = &attr_md;
  attr_char_value.init_len = 0;
  attr_char_value.max_len = 3;

  err_code = sd_ble_gatts_characteristic_add(service_light->service_handle, &char_md, &attr_char_value,
      &service_light->command_char_handles);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  return NRF_SUCCESS;
}

uint32_t ble_light_init(service_light_t *service_light) {
  uint32_t err_code;
  ble_uuid_t ble_uuid;

  service_light->conn_handle = BLE_CONN_HANDLE_INVALID;

  ble_uuid128_t base_uuid = { SERVICE_LIGHT_UUID_BASE };
  err_code = sd_ble_uuid_vs_add(&base_uuid, &service_light->uuid_type);
  APP_ERROR_CHECK(err_code);

  ble_uuid.type = service_light->uuid_type;
  ble_uuid.uuid = SERVICE_LIGHT_UUID;

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &service_light->service_handle);
  APP_ERROR_CHECK(err_code);

  err_code = command_char_add(service_light);
  APP_ERROR_CHECK(err_code);

  return NRF_SUCCESS;
}

void service_light_init(void) {
  ret_code_t err_code;
  err_code = ble_light_init(&m_light_service);
  APP_ERROR_CHECK(err_code);
}
