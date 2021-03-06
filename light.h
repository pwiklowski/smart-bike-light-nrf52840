/*
 * light.h
 *
 *  Created on: Mar 19, 2021
 *      Author: pwiklowski
 */

#ifndef MAIN_LIGHT_H_
#define MAIN_LIGHT_H_

void light_init();
void light_set_value(uint16_t char_uuid, const uint8_t* data, const uint16_t len);

void light_handle_message(uint8_t* message, uint16_t len);


#endif /* MAIN_LIGHT_H_ */
