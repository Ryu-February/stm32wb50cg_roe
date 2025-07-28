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


bool led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	for(int i = 0; i < LED_MAX_CH; i++)
	{
		GPIO_InitStruct.Pin = led_table[i].pin;
		HAL_GPIO_Init(led_table[i].port, &GPIO_InitStruct);

		led_off(i);
	}

	return true;
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
