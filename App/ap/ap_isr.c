/*
 * ap_isr.c
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */


#include "ap_isr.h"
#include "rgb.h"



void ap_tim2_callback(void)
{

}


void ap_tim16_callback(void)
{
	rgb_set_color(COLOR_PINK);
}

void ap_tim17_callback(void)
{

}
