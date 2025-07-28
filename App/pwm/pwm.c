/*
 * pwm.c
 *
 *  Created on: Jul 25, 2025
 *      Author: fbcks
 */


#include "pwm.h"




bool pwm_init(void)
{
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_Base_Start_IT(&htim17);

	return true;
}

void pwm_write(uint8_t ch, uint16_t duty)
{
	switch(ch)
	{
		case _DEF_CH1 :
			break;
	}
}

uint16_t pwm_read(uint8_t ch)
{
	uint16_t pwm_data = 0;

	return pwm_data;
}
