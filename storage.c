/*
 * storage.c
 *
 *  Created on: Apr 3, 2021
 *      Author: pwiklowski
 */

#include <string.h>
#include "nrf.h"
#include "nrf_log.h"
#include "nordic_common.h"
#include "fds.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"
#include "FreeRTOS.h"
#include "task.h"

#define CONFIG_FILE     (0x8010)
#define CONFIG_REC_KEY  (0x7010)

static void fds_evt_handler(fds_evt_t const *const p_fds_evt)
{
  switch (p_fds_evt->id)
  {
    case FDS_EVT_INIT:
      NRF_LOG_INFO("FDS_EVT_INIT");

      if (p_fds_evt->result == NRF_SUCCESS) {
        ret_code_t rc;

        fds_gc();
        NRF_LOG_INFO("Reading flash usage statistics...");

        fds_stat_t stat = { 0 };

        rc = fds_stat(&stat);
        APP_ERROR_CHECK(rc);

        NRF_LOG_INFO("Found %d valid records.", stat.valid_records);
        NRF_LOG_INFO("Found %d dirty records (ready to be garbage collected).", stat.dirty_records);
      }
      break;
    case FDS_EVT_WRITE:

      break;
    default:
      break;
  }
}


void storage_init() {
  ret_code_t rc;

  rc = fds_register(fds_evt_handler);
  APP_ERROR_CHECK(rc);

  rc = fds_init();
  APP_ERROR_CHECK(rc);


}

uint32_t round_up_u32(uint32_t len) {
  if (len % sizeof(uint32_t)) {
    return (len + sizeof(uint32_t) - (len % sizeof(uint32_t)));
  }
  return len;
}

void storage_write(const uint8_t *data, const uint32_t len) {
  ret_code_t rc;

  fds_record_t const record = {
    .file_id = CONFIG_FILE,
    .key = CONFIG_REC_KEY,
    .data.p_data = data,
    /* The length of a record is always expressed in 4-byte units (words). */
    .data.length_words = (len + 3) / 4,
  };
  fds_record_desc_t desc = { 0 };
  fds_find_token_t tok = { 0 };

  rc = fds_record_find(CONFIG_FILE, CONFIG_REC_KEY, &desc, &tok);

  if (rc == NRF_SUCCESS) {
    rc = fds_record_update(&desc, &record);
    if ((rc != NRF_SUCCESS) && (rc == FDS_ERR_NO_SPACE_IN_FLASH)) {
      NRF_LOG_INFO("No space in flash, delete some records to update the config file.");
    } else {
      APP_ERROR_CHECK(rc);
    }
  } else {
    rc = fds_record_write(&desc, &record);
    if ((rc != NRF_SUCCESS) && (rc == FDS_ERR_NO_SPACE_IN_FLASH)) {
      NRF_LOG_INFO("No space in flash, delete some records to update the config file.");
    } else {
      APP_ERROR_CHECK(rc);
    }
  }
}

void storage_read(uint8_t *data, uint32_t len) {
  ret_code_t rc;

  fds_record_desc_t desc = { 0 };
  fds_find_token_t tok = { 0 };

  rc = fds_record_find(CONFIG_FILE, CONFIG_REC_KEY, &desc, &tok);

  if (rc == NRF_SUCCESS) {
    fds_flash_record_t config = { 0 };

    rc = fds_record_open(&desc, &config);
    APP_ERROR_CHECK(rc);

    memcpy(data, config.p_data, config.p_header->length_words * 4);

    rc = fds_record_close(&desc);
    APP_ERROR_CHECK(rc);
  } else {
    memset(data, 0, len);
  }
}
