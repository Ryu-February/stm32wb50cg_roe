/*
 * input.h
 *
 *  Created on: Jul 30, 2025
 *      Author: fbcks
 */

#ifndef INPUT_INPUT_H_
#define INPUT_INPUT_H_

#include "def.h"


#define NUM_INPUTS 7

typedef enum
{
	SYS_IDLE,
	SYS_WAIT_RELEASE,
	SYS_SHORT_PRESS,
	SYS_LONG_PRESS,
	SYS_CALIBRATE
}sys_state_t;

typedef enum		//스위치 모드 예시임(이건 수정해도 됨)
{
	INPUT_MODE,      // ex: 모드 변경 버튼
	INPUT_SELECT,
	INPUT_START,
	INPUT_BACK,
	INPUT_OK,
	INPUT_CANCEL,
	INPUT_RECALIBRATE,

	INPUT_COUNT           // 전체 스위치 개수 (for bounds check)
} input_id_t;


void input_exti_triggered(input_id_t id, bool level);
void input_update(void);
bool input_is_short_pressed(input_id_t id);
bool input_is_long_pressed(input_id_t id);

#endif /* INPUT_INPUT_H_ */
