/*
 * ap.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */


#include "ap.h"

extern ADC_HandleTypeDef hadc1;




void ap_init(void)
{
	led_init();
	pwm_init();
	rgb_init();


	led_on(_DEF_CH1);//white led on
}


void ap_main(void)
{
	uint32_t prev_time = 0;


	while(1)
	{
		if (millis() - prev_time >= 500)
		{
			prev_time = millis();
			led_toggle(_DEF_CH1);
		}
	}
}
