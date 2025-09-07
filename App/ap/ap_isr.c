/*
 * ap_isr.c
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */


#include "ap_isr.h"
#include "rgb.h"
#include "color.h"
#include "input.h"
#include "step.h"

volatile uint32_t timer17_ms;
volatile bool check_color;
volatile bool mode_update;
volatile bool rgb_update;
volatile bool step_update;

extern volatile uint8_t detected_color;
extern volatile uint32_t target_steps;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		case GPIO_PIN_0:
			ap_exti0_callback();
			break;
	}
}

void ap_exti0_callback(void)
{
	bool level = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET);  // Pull-up 기준
	input_exti_triggered(INPUT_MODE, level);

	if(level)
		check_color = true;
}


void ap_tim2_callback(void)
{

}

void ap_tim16_callback(void)
{
//	rgb_set_color(detected_color);

//	static uint32_t prev_ms = 0;
//	static bool mode_timing_started = false;
//
//	if(mode_update)
//	{
//		uint32_t cur_ms = millis();
//
//		if(!mode_timing_started)
//		{
//			prev_ms = cur_ms;
//			mode_timing_started = true;
//			return;
//		}
//
//		if(cur_ms - prev_ms < 500)
//		{
//			return;
//		}
//
//		mode_timing_started = false;
//		mode_update = false;
//	}

	rgb_set_color(detected_color);

	if(detected_color == COLOR_BLACK)
	{
		return;
	}

	switch (detected_color)
	{
		case COLOR_RED :
			step_drive(REVERSE);
			target_steps = 1500;
			break;
		case COLOR_GREEN :
			step_drive(FORWARD);
			target_steps = 1500;
			break;
		case COLOR_BLUE :
			step_drive(TURN_LEFT);
			target_steps = 1050;
			break;
		case COLOR_YELLOW :
			step_drive(TURN_RIGHT);
			target_steps = 1050;
			break;
	}

	if(get_current_steps() >= target_steps)
	{
		detected_color = COLOR_BLACK;
		executed_step_init();
		step_drive(STOP);
	}
}

void ap_tim17_callback(void)
{
	timer17_ms++;

	input_update();
}
