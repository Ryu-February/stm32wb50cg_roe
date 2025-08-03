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
	rgb_update = true;

	if(detected_color != COLOR_BLACK)
	{
		step_update = true;
	}
//	switch (detected_color)
//	{
//		case COLOR_RED :
////			step_drive(FORWARD);
//			apply_test(LEFT);
//			apply_test(RIGHT);
//			break;
//		case COLOR_ORANGE :
//			step_drive(REVERSE);
//			break;
//		case COLOR_YELLOW :
//			step_drive(TURN_LEFT);
//			break;
//		case COLOR_GREEN :
//			step_drive(TURN_RIGHT);
//			break;
//	}
}

void ap_tim17_callback(void)
{
	timer17_ms++;

	input_update();
}
