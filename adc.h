/*
 * adc.h
 *
 *  Created on: Apr 19, 2021
 *      Author: pwiklowski
 */

#ifndef ADC_H_
#define ADC_H_


void battery_voltage_init(void);
void battery_voltage_get(uint16_t * p_vbatt);


#endif /* ADC_H_ */
