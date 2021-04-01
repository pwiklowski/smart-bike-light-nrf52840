/*
 * logger.c
 *
 *  Created on: Apr 1, 2021
 *      Author: pwiklowski
 */
#include "app_error.h"
#include "FreeRTOS.h"
#include "task.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


void log_init(void) {
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void logger_thread(void *arg) {
  UNUSED_PARAMETER(arg);

  while (1) {
    NRF_LOG_FLUSH();
    vTaskSuspend(NULL); // Suspend myself
  }
}
