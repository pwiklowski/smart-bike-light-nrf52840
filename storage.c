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

bool is_writing = false;
void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) = {
  /* Set a handler for fstorage events. */
  .evt_handler = fstorage_evt_handler,

  /* These below are the boundaries of the flash space assigned to this instance of fstorage.
   * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
   * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
   * last page of flash available to write data. */
  .start_addr = 0x3e000,
  .end_addr = 0x3ffff,
};

static uint32_t nrf5_flash_end_addr_get()
{
  uint32_t const bootloader_addr = BOOTLOADER_ADDRESS;
  uint32_t const page_sz = NRF_FICR->CODEPAGESIZE;
  uint32_t const code_sz = NRF_FICR->CODESIZE;

  return (bootloader_addr != 0xFFFFFFFF ? bootloader_addr : (code_sz * page_sz));
}

void print_flash_info(nrf_fstorage_t * p_fstorage) {
    NRF_LOG_INFO("========| flash info |========");
    NRF_LOG_INFO("erase unit: \t%d bytes", p_fstorage->p_flash_info->erase_unit);
    NRF_LOG_INFO("program unit: \t%d bytes", p_fstorage->p_flash_info->program_unit);
    NRF_LOG_INFO("flash end unit: \t%d bytes", nrf5_flash_end_addr_get());
    NRF_LOG_INFO("==============================");
}

void storage_init() {
  ret_code_t rc;
  nrf_fstorage_api_t *p_fs_api;

  p_fs_api = &nrf_fstorage_sd;

  rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
  APP_ERROR_CHECK(rc);

  print_flash_info(&fstorage);
}

void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt) {
  if (p_evt->result != NRF_SUCCESS) {
    NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
    return;
  }

  switch (p_evt->id) {
    case NRF_FSTORAGE_EVT_WRITE_RESULT: {
      NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.", p_evt->len, p_evt->addr);
      break;
    }
    case NRF_FSTORAGE_EVT_ERASE_RESULT: {
      NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.", p_evt->len, p_evt->addr);
      break;
    }
    default:
      break;
  }
}

void wait_for_flash_ready(nrf_fstorage_t const *p_fstorage) {
  //TODO
}

uint32_t round_up_u32(uint32_t len) {
  if (len % sizeof(uint32_t)) {
    return (len + sizeof(uint32_t) - (len % sizeof(uint32_t)));
  }
  return len;
}


void storage_write(uint8_t* data, uint32_t len) {
  NRF_LOG_INFO("storage_write %d", len);
  ret_code_t rc;

  uint32_t rounded_len = round_up_u32(len);

  is_writing = true;
  rc = nrf_fstorage_write(&fstorage, 0x3e000, data, rounded_len, NULL);
  APP_ERROR_CHECK(rc);

  wait_for_flash_ready(&fstorage);
  NRF_LOG_INFO("Done.");
}

void storage_read(uint8_t* data, uint32_t len) {
  NRF_LOG_INFO("storage_read %d", len);
  ret_code_t rc;
  rc = nrf_fstorage_read(&fstorage, 0x3e000, data, len);
  APP_ERROR_CHECK(rc);
}
