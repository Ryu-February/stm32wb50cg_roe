/*
 * utils.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */


#include "utils.h"





void delay(uint32_t ms)
{
	HAL_Delay(ms);
}


uint32_t millis(void)
{
	return HAL_GetTick();
}
