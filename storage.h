/*
 * storage.h
 *
 *  Created on: Apr 3, 2021
 *      Author: pwiklowski
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include <stdint.h>


void storage_init();
void storage_write(uint8_t* data, uint32_t len);
void storage_read(uint8_t* data, uint32_t len);


#endif /* STORAGE_H_ */
