/*
 * ir.h
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */

#ifndef IR_IR_H_
#define IR_IR_H_


#include "def.h"

#define IR_THRESHOLD 	30


void ir_init(void);


uint16_t ir_read_adc(void);
uint8_t ir_is_black(void);



#endif /* IR_IR_H_ */
