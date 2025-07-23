/*
 * ap.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */


#include "ap.h"





void ap_init(void)
{
	led_init();
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
