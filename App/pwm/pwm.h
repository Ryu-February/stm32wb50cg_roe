/*
 * pwm.h
 *
 *  Created on: Jul 25, 2025
 *      Author: fbcks
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_







#include "def.h"


extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;


bool pwm_init(void);
void pwm_write(uint8_t ch, uint16_t duty);
uint16_t pwm_read(uint8_t ch);


#endif /* INC_PWM_H_ */
