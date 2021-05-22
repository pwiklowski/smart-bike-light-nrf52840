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
#include "light.h"
#include "ble_conn_state.h"

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"


MessageBufferHandle_t xMessageBuffer;

typedef struct {
  uint16_t message_id;
  uint16_t message_len;
} Header;


service_light_t m_light_service;
NRF_SDH_BLE_OBSERVER(m_light_service_obs, SERVICE_LIGHT_BLE_OBSERVER_PRIO, service_light_on_ble_evt, &m_light_service);

ret_code_t service_light_send_notification(uint8_t *data, uint16_t len) {
  ble_gatts_hvx_params_t hvx_params;

  memset(&hvx_params, 0, sizeof(hvx_params));

  hvx_params.handle = m_light_service.handle_front_light_config.value_handle;
  hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
  hvx_params.offset = 0;
  hvx_params.p_len = &len;
  hvx_params.p_data = (uint8_t*) data;

  return sd_ble_gatts_hvx(m_light_service.conn_handle, &hvx_params);
}

void service_light_send_response(uint16_t messageId, uint8_t *data, uint16_t len) {
  ret_code_t err_code;
  uint8_t notification_size = 20;

  uint16_t data_sent = 0;

  Header header = {
      .message_id = messageId,
      .message_len = len
  };

  err_code = service_light_send_notification((uint8_t*) &header, sizeof(header));

  while (data_sent < len) {
    uint8_t chunk_size = len - data_sent > notification_size ? notification_size : len - data_sent;
    err_code = service_light_send_notification(&data[data_sent], chunk_size);

    if (err_code == NRF_ERROR_RESOURCES) {
      NRF_LOG_INFO("TX BUFFER FULL");
      vTaskDelay(50);

    } else {
      data_sent += chunk_size;
    }
  }
}

void service_light_message_handler() {
  uint8_t message[512];
  size_t len;
  const TickType_t xBlockTime = pdMS_TO_TICKS(100);

  while (1) {
    len = xMessageBufferReceive(xMessageBuffer, message, sizeof(message), xBlockTime);

    if (len > 0) {
      light_handle_message(message, len);
    }
  }
}

void service_light_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context) {
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
    case BLE_GATTS_EVT_WRITE: {
      ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
      if (p_evt_write->uuid.uuid == CHAR_UUID_FRONT_CONFIG) {
        NRF_LOG_INFO("WRITE %d %s", p_evt_write->len, &p_evt_write->data[2]);
        xMessageBufferSend(xMessageBuffer, p_evt_write->data, p_evt_write->len, 100);
      } else {
        light_set_value(p_evt_write->uuid.uuid, p_evt_write->data, p_evt_write->len);
      }
      break;

      case BLE_GATTS_EVT_HVN_TX_COMPLETE:
      NRF_LOG_INFO("BLE_GATTS_EVT_HVN_TX_COMPLETE");

    }
    default:
      // No implementation needed.
      break;
  }
}

uint32_t service_light_add_characteristic(service_light_t *service_light, uint16_t char_uuid, uint16_t len,
    ble_gatts_char_handles_t *handle, uint8_t notify_enabled) {
  uint32_t err_code;

  ble_gatts_char_md_t char_md = {
  .char_props.read = 1,
  .char_props.write = 1,
  .char_props.notify = notify_enabled,
  };

  ble_uuid_t ble_uuid = {
  .type = service_light->uuid_type,
  .uuid = char_uuid };

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

  err_code = sd_ble_gatts_characteristic_add(service_light->service_handle,
      &char_md,
      &attr_char_value,
      handle);

  return err_code;
}

uint32_t service_light_config_characteristic(service_light_t *service_light, uint16_t char_uuid,
    ble_gatts_char_handles_t *handle) {
  uint32_t err_code;

  ble_gatts_char_md_t char_md = {
  .char_props.read = 0,
  .char_props.write = 1,
  .char_props.notify = 1,
  };

  ble_uuid_t ble_uuid = {
  .type = service_light->uuid_type,
  .uuid = char_uuid
  };

  ble_gatts_attr_md_t attr_md = {
  .vloc = BLE_GATTS_VLOC_STACK,
  .rd_auth = 0,
  .wr_auth = 0,
  .vlen = 1
  };

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

  ble_gatts_attr_t attr_char_value = {
  .p_uuid = &ble_uuid,
  .p_attr_md = &attr_md,
  .init_len = 0,
  .max_len = 256 };

  err_code = sd_ble_gatts_characteristic_add(service_light->service_handle,
      &char_md,
      &attr_char_value,
      handle);

  return err_code;
}

uint32_t ble_light_init(service_light_t *service_light) {
  uint32_t err_code;

  xMessageBuffer = xMessageBufferCreate(1024);
  xTaskCreate(service_light_message_handler, "LOGGER", 2048, NULL, 1, NULL);


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

  err_code = service_light_add_characteristic(service_light, CHAR_UUID_FRONT_LIGHT_TOGGLE, 1,
      &service_light->handle_front_light_toggle, 1);
  APP_ERROR_CHECK(err_code);
  err_code = service_light_add_characteristic(service_light, CHAR_UUID_FRONT_LIGHT_MODE, 1,
      &service_light->handle_front_light_mode, 1);
  APP_ERROR_CHECK(err_code);
  err_code = service_light_add_characteristic(service_light, CHAR_UUID_FRONT_LIGHT_SETTING, 4,
      &service_light->handle_front_light_setting, 0);
  APP_ERROR_CHECK(err_code);

  err_code = service_light_add_characteristic(service_light, CHAR_UUID_BACK_LIGHT_TOGGLE, 1,
      &service_light->handle_back_light_toggle, 1);
  APP_ERROR_CHECK(err_code);
  err_code = service_light_add_characteristic(service_light, CHAR_UUID_BACK_LIGHT_MODE, 1,
      &service_light->handle_back_light_mode, 1);
  APP_ERROR_CHECK(err_code);
  err_code = service_light_add_characteristic(service_light, CHAR_UUID_BACK_LIGHT_SETTING, 4,
      &service_light->handle_back_light_setting, 0);
  APP_ERROR_CHECK(err_code);

  err_code = service_light_config_characteristic(service_light, CHAR_UUID_FRONT_CONFIG,
      &service_light->handle_front_light_config);
  APP_ERROR_CHECK(err_code);

  return NRF_SUCCESS;
}

void service_light_init(void) {
  ret_code_t err_code;
  err_code = ble_light_init(&m_light_service);
  APP_ERROR_CHECK(err_code);
}

ret_code_t notification_send(ble_gatts_hvx_params_t *const p_hvx_params, uint16_t conn_handle) {
  ret_code_t err_code = sd_ble_gatts_hvx(conn_handle, p_hvx_params);
  if (err_code == NRF_SUCCESS) {
    NRF_LOG_INFO("notification has been sent using conn_handle: 0x%04X", conn_handle);
  } else {
    NRF_LOG_DEBUG("Error: 0x%08X while sending notification with conn_handle: 0x%04X", err_code, conn_handle);
  }
  return err_code;
}

void service_light_value_update(ble_gatts_char_handles_t *handle, uint8_t *data,
    uint16_t data_len, bool notify) {

  ret_code_t err_code = NRF_SUCCESS;
  ble_gatts_value_t gatts_value;

  gatts_value.len = data_len;
  gatts_value.offset = 0;
  gatts_value.p_value = data;

  UNUSED_PARAMETER(gatts_value);

  // Update database.
  err_code = sd_ble_gatts_value_set(m_light_service.conn_handle, handle->value_handle, &gatts_value);
  if (err_code == NRF_SUCCESS) {
    NRF_LOG_INFO("Battery level has been updated: %d%%");
  } else {
    NRF_LOG_DEBUG("Error during battery level update: 0x%08X", err_code)
  }

  // Send value if connected and notifying.
  if (notify) {
    ble_gatts_hvx_params_t hvx_params;

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = handle->value_handle;
    hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = gatts_value.offset;
    hvx_params.p_len = &gatts_value.len;
    hvx_params.p_data = gatts_value.p_value;

    notification_send(&hvx_params, m_light_service.conn_handle);
  }
  //}
}

uint8_t service_light_get_toggle_front() {
  ret_code_t err_code = NRF_SUCCESS;
  ble_gatts_value_t gatts_value;
  uint8_t val;

  gatts_value.len = 1;
  gatts_value.offset = 0;
  gatts_value.p_value = &val;

  err_code = sd_ble_gatts_value_get(m_light_service.conn_handle, m_light_service.handle_front_light_toggle.value_handle,
      &gatts_value);

  if (err_code != NRF_SUCCESS) {
    NRF_LOG_INFO("Error reading value 0x%08X", err_code)
  }

  return val;
}

uint8_t service_light_get_toggle_back() {
  ret_code_t err_code = NRF_SUCCESS;
  ble_gatts_value_t gatts_value;

  uint8_t val;

  gatts_value.len = 1;
  gatts_value.offset = 0;
  gatts_value.p_value = &val;

  err_code = sd_ble_gatts_value_get(m_light_service.conn_handle, m_light_service.handle_back_light_toggle.value_handle,
      &gatts_value);

  if (err_code != NRF_SUCCESS) {
    NRF_LOG_DEBUG("Error reading value 0x%08X", err_code)
  }

  return val;
}

void service_light_update(uint16_t char_uuid, uint8_t *data, uint16_t len) {
  switch (char_uuid) {
    case CHAR_UUID_FRONT_LIGHT_TOGGLE:
      service_light_value_update(&m_light_service.handle_front_light_toggle, data, len, 1);
      break;
    case CHAR_UUID_FRONT_LIGHT_MODE:
      service_light_value_update(&m_light_service.handle_front_light_mode, data, len, 1);
      break;
    case CHAR_UUID_FRONT_LIGHT_SETTING:
      service_light_value_update(&m_light_service.handle_front_light_setting, data, len, 0);
      break;
    case CHAR_UUID_BACK_LIGHT_TOGGLE:
      service_light_value_update(&m_light_service.handle_back_light_toggle, data, len, 1);
      break;
    case CHAR_UUID_BACK_LIGHT_MODE:
      service_light_value_update(&m_light_service.handle_back_light_mode, data, len, 1);
      break;
    case CHAR_UUID_BACK_LIGHT_SETTING:
      service_light_value_update(&m_light_service.handle_back_light_setting, data, len, 0);
      break;
  }

  light_set_value(char_uuid, data, len);
}
