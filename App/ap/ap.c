/*
 * ap.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */


#include "ap.h"






void ap_init(void)
{
	i2c_init();
	uart_init();

	ir_init();
	led_init();
	pwm_init();
	rgb_init();
	color_init();
	step_motor_init();
}


void ap_main(void)
{
	while(1)
	{
	}
}
