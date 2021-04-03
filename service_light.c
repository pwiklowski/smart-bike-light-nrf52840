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
      NRF_LOG_INFO("BLE_GAP_EVT_CONNECTED") ;
      service_light->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
      break;
    case BLE_GAP_EVT_DISCONNECTED:
      NRF_LOG_INFO("BLE_GAP_EVT_DISCONNECTED") ;
      service_light->conn_handle = BLE_CONN_HANDLE_INVALID;
      break;
    case BLE_GATTS_EVT_WRITE: {
      ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
      NRF_LOG_INFO("BLE_GATTS_EVT_WRITE %d", p_evt_write->len);
      break;
    }
    default:
      // No implementation needed.
      break;
  }
}

uint32_t service_light_add_characteristic(service_light_t *service_light, uint16_t char_uuid, uint8_t len) {
  uint32_t err_code;

  ble_gatts_char_md_t char_md = {
    .char_props.read = 1,
    .char_props.write = 1,
    .char_props.notify = 0,
  };

  ble_uuid_t ble_uuid = {
    .type = service_light->uuid_type,
    .uuid = char_uuid};

  ble_gatts_attr_md_t attr_md = {
    .vloc = BLE_GATTS_VLOC_STACK,
    .rd_auth = 0,
    .wr_auth = 0,
    .vlen = 0
  };

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

  ble_gatts_attr_t attr_char_value = {
    .p_uuid = &ble_uuid,
    .p_attr_md = &attr_md,
    .init_len = 0,
    .max_len = len
  };

  err_code = sd_ble_gatts_characteristic_add(service_light->service_handle, &char_md, &attr_char_value, &service_light->command_char_handles);

  return err_code;
}

uint32_t ble_light_init(service_light_t *service_light) {
  uint32_t err_code;

  service_light->conn_handle = BLE_CONN_HANDLE_INVALID;

  ble_uuid128_t base_uuid = { SERVICE_LIGHT_UUID_BASE };
  err_code = sd_ble_uuid_vs_add(&base_uuid, &service_light->uuid_type);
  APP_ERROR_CHECK(err_code);

  ble_uuid_t ble_uuid = {
    .type = service_light->uuid_type,
    .uuid = SERVICE_LIGHT_UUID
  };

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &service_light->service_handle);
  APP_ERROR_CHECK(err_code);

  err_code = service_light_add_characteristic(service_light, CHAR_UUID_FRONT_LIGHT_TOGGLE, 1);
  APP_ERROR_CHECK(err_code);
  err_code = service_light_add_characteristic(service_light, CHAR_UUID_FRONT_LIGHT_MODE, 1);
  APP_ERROR_CHECK(err_code);
  err_code = service_light_add_characteristic(service_light, CHAR_UUID_FRONT_LIGHT_SETTING, 3);
  APP_ERROR_CHECK(err_code);

  err_code = service_light_add_characteristic(service_light, CHAR_UUID_BACK_LIGHT_TOGGLE, 1);
  APP_ERROR_CHECK(err_code);
  err_code = service_light_add_characteristic(service_light, CHAR_UUID_BACK_LIGHT_MODE, 1);
  APP_ERROR_CHECK(err_code);
  err_code = service_light_add_characteristic(service_light, CHAR_UUID_BACK_LIGHT_SETTING, 3);
  APP_ERROR_CHECK(err_code);

  return NRF_SUCCESS;
}

void service_light_init(void) {
  ret_code_t err_code;
  err_code = ble_light_init(&m_light_service);
  APP_ERROR_CHECK(err_code);
}
