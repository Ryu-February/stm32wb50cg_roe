/*
 * i2c.h
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_


#include "def.h"


void i2c_init(void);
void i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
uint8_t i2c_read(uint8_t slave_addr, uint8_t reg_addr);


#endif /* I2C_I2C_H_ */
