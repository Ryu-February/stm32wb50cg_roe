/*
 * led.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */


#include "led.h"





bool led_init(void)
{
	return true;
}

void led_on(uint8_t ch)
{
	switch(ch)
	{
		case _DEF_CH1 :
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
			break;
	}
}

void led_off(uint8_t ch)
{
	switch(ch)
	{
		case _DEF_CH1 :
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
			break;
	}
}

void led_toggle(uint8_t ch)
{
	switch(ch)
	{
		case _DEF_CH1 :
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
			break;
	}
}
