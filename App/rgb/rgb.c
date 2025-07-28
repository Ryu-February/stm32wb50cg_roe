/*
 * rgb.c
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */


#include "rgb.h"



const rgb_led_t led_map[COLOR_COUNT] =
{
		[COLOR_RED]         = { 255,   0,   0 },
		[COLOR_ORANGE]      = { 255, 165,   0 },
		[COLOR_YELLOW]      = { 255, 255,   0 },
		[COLOR_GREEN]       = {   0, 255,   0 },
		[COLOR_BLUE]        = {   0,   0, 255 },
		[COLOR_PURPLE]      = { 160,  32, 240 },  // or VIOLET
		[COLOR_LIGHT_GREEN] = {  26, 255,  26 },
		[COLOR_SKY_BLUE]    = {  70, 200, 255 },  // LIGHT_BLUE or CYAN mix
		[COLOR_PINK]        = { 255, 105, 180 },
		[COLOR_BLACK]       = {   0,   0,   0 },
		[COLOR_WHITE]       = { 255, 255, 255 },
		[COLOR_GRAY]        = { 128, 128, 128 }
};


void rgb_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();


	/*Configure GPIO pins : PA4 PA5 PA6 */
	GPIO_InitStruct.Pin = RGB_CH_BLUE|RGB_CH_RED|RGB_CH_GREEN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, RGB_CH_BLUE | RGB_CH_RED | RGB_CH_GREEN, GPIO_PIN_SET);
}

void rgb_set_pwm(uint8_t r, uint8_t g, uint8_t b)
{
	static uint8_t pwm_period = 0;

	if(++pwm_period >= 255)
	{
		pwm_period = 0;
	}

	if(pwm_period > 255 - r)
	{
		HAL_GPIO_WritePin(GPIOA, RGB_CH_RED, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, RGB_CH_RED, GPIO_PIN_SET);
	}

	if(pwm_period > 255 - g)
	{
		HAL_GPIO_WritePin(GPIOA, RGB_CH_GREEN, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, RGB_CH_GREEN, GPIO_PIN_SET);
	}

	if(pwm_period > 255 - b)
	{
		HAL_GPIO_WritePin(GPIOA, RGB_CH_BLUE, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, RGB_CH_BLUE, GPIO_PIN_SET);
	}

}

void rgb_set_color(color_t color)
{
	if(color >= COLOR_COUNT)
		return;

	rgb_set_pwm(led_map[color].r, led_map[color].g, led_map[color].b);
}
