/*
 * adc.c
 *
 *  Created on: Apr 17, 2021
 *      Author: pwiklowski
 */
#include "nrf_drv_saadc.h"
#include "sdk_macros.h"
#include "nrf_log.h"

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS  600  //!< Reference voltage (in milli volts) used by ADC while doing conversion.
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS 270  //!< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com.
#define ADC_RES_10BIT                  4096 //!< Maximum digital value for 10-bit ADC conversion.
#define ADC_PRE_SCALING_COMPENSATION   6    //!< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE) \
    ((((ADC_VALUE) *ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_PRE_SCALING_COMPENSATION)

static nrf_saadc_value_t adc_buf;                   //!< Buffer used for storing ADC value.
static uint16_t m_batt_lvl_in_milli_volts; //!< Current battery level.

/**@brief Function handling events from 'nrf_drv_saadc.c'.
 *
 * @param[in] p_evt SAADC event.
 */
static void saadc_event_handler(nrf_drv_saadc_evt_t const *p_evt)
{
  if (p_evt->type == NRF_DRV_SAADC_EVT_DONE)
  {
    nrf_saadc_value_t adc_result;

    adc_result = p_evt->data.done.p_buffer[0];

    m_batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result);

    NRF_LOG_INFO("ADC reading - ADC:%d,  In Millivolts: %d\r\n", adc_result, m_batt_lvl_in_milli_volts);
  }
}

void battery_voltage_init(void) {
  nrf_drv_saadc_config_t saadc_config;
  saadc_config.low_power_mode = true;                                                   //Enable low power mode.
  saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;                                 //Set SAADC resolution to 12-bit. This will make the SAADC output values from 0 (when input voltage is 0V) to 2^12=4096 (when input voltage is 3.6V for channel gain setting of 1/6).
  saadc_config.oversample = NRF_SAADC_OVERSAMPLE_64X;                                           //Set oversample to 4x. This will make the SAADC output a single averaged value when the SAMPLE task is triggered 4 times.
  saadc_config.interrupt_priority = APP_IRQ_PRIORITY_LOW;

  ret_code_t err_code = nrf_drv_saadc_init(&saadc_config, saadc_event_handler);

  APP_ERROR_CHECK(err_code);

  nrf_saadc_channel_config_t config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);
  config.gain = NRF_SAADC_GAIN1_6;
  config.burst = NRF_SAADC_BURST_ENABLED;
  err_code = nrf_drv_saadc_channel_init(0, &config);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_buffer_convert(&adc_buf, 1);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_saadc_sample();
  APP_ERROR_CHECK(err_code);
}

void battery_voltage_get(uint16_t *p_vbatt) {
  VERIFY_PARAM_NOT_NULL_VOID(p_vbatt);

  *p_vbatt = m_batt_lvl_in_milli_volts;
  if (!nrf_drv_saadc_is_busy()) {
    ret_code_t err_code = nrf_drv_saadc_buffer_convert(&adc_buf, 1);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_sample();
    APP_ERROR_CHECK(err_code);
  }
}
