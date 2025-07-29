/*
 * led.h
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */

#ifndef INC_LED_H_
#define INC_LED_H_



#include "def.h"



void led_init(void);

void led_on(uint8_t ch);
void led_off(uint8_t ch);
void led_toggle(uint8_t ch);

#endif /* INC_LED_H_ */

