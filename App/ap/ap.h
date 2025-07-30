/*
 * ap.h
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */

#ifndef AP_H_
#define AP_H_


#include "utils.h"

#include "ir.h"
#include "led.h"
#include "pwm.h"
#include "rgb.h"
#include "i2c.h"
#include "uart.h"
#include "step.h"
#include "flash.h"
#include "color.h"
#include "input.h"



void ap_init(void);
void ap_main(void);





#endif /* AP_H_ */
