/*
 * led.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */


#include "led.h"


typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_PinState on_state;
    GPIO_PinState off_state;
}led_table_t;

led_table_t led_table[LED_MAX_CH] =
{
		{GPIOB, GPIO_PIN_1, GPIO_PIN_SET, GPIO_PIN_RESET},
};


void led_init(void)
{
	led_on(_DEF_CH1);	//white led on(TR Base)
}

void led_on(uint8_t ch)
{
	if(ch >= LED_MAX_CH)
		return;

	switch(ch)
	{
		case _DEF_CH1 :
			HAL_GPIO_WritePin(led_table[ch].port, led_table[ch].pin, led_table[ch].on_state);
			break;
	}
}

void led_off(uint8_t ch)
{
	if(ch >= LED_MAX_CH)
		return;

	switch(ch)
	{
		case _DEF_CH1 :
			HAL_GPIO_WritePin(led_table[ch].port, led_table[ch].pin, led_table[ch].off_state);
			break;
	}
}

void led_toggle(uint8_t ch)
{
	if(ch >= LED_MAX_CH)
		return;

	switch(ch)
	{
		case _DEF_CH1 :
			HAL_GPIO_TogglePin(led_table[ch].port, led_table[ch].pin);
			break;
	}
}
