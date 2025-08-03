/*
 * step.c
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */


#include "step.h"

extern TIM_HandleTypeDef htim2;

volatile bool idx_change = false;
volatile StepOperation step_op;

/*                            Motor(15BY25-119)                         */
/*                           Motor driver(A3916)                        */

/************************************************************************/
/*                       MCU  |     NET    | DRIVER                     */
/*                       PA0 -> MOT_L_IN1 ->  IN1                       */
/*                       PA1 -> MOT_L_IN2 ->  IN2                       */
/*                       PA2 -> MOT_L_IN3 ->  IN3                       */
/*                       PA3 -> MOT_L_IN4 ->  IN4                       */
/*                                                                      */
/*                       PB4 -> MOT_R_IN1 ->  IN1                       */
/*                       PB5 -> MOT_R_IN2 ->  IN2                       */
/*                       PB6 -> MOT_R_IN3 ->  IN3                       */
/*                       PB7 -> MOT_R_IN4 ->  IN4                       */
/************************************************************************/

/*
 * A3916 Stepper Motor Operation Table (Half Step + Full Step)
 *
 * IN1 IN2 IN3 IN4 | OUT1A OUT1B OUT2A OUT2B | Function
 * --------------------------------------------------------
 *  0   0   0   0  |  Off   Off   Off   Off   | Disabled
 *  1   0   1   0  |  High  Low   High  Low   | Full Step 1 / 	½ Step 1
 *  0   0   1   0  |  Off   Off   High  Low   |             	½ Step 2
 *  0   1   1   0  |  Low   High  High  Low   | Full Step 2 /	½ Step 3
 *  0   1   0   0  |  Low   High  Off   Off   |             	½ Step 4
 *  0   1   0   1  |  Low   High  Low   High  | Full Step 3 / 	½ Step 5
 *  0   0   0   1  |  Off   Off   Low   High  |             	½ Step 6
 *  1   0   0   1  |  High  Low   Low   High  | Full Step 4 / 	½ Step 7
 *  1   0   0   0  |  High  Low   Off   Off   |             	½ Step 8
 */

DEFINE_STEP_MOTOR(step_motor_left,
	GPIOA, GPIO_PIN_0,   // IN1: PA0
	GPIOA, GPIO_PIN_1,   // IN2: PA1
	GPIOA, GPIO_PIN_2,   // IN3: PA2
	GPIOA, GPIO_PIN_3    // IN4: PA3
);

DEFINE_STEP_MOTOR(step_motor_right,
	GPIOB, GPIO_PIN_4,   // IN1: PB4
	GPIOB, GPIO_PIN_5,   // IN2: PB5
	GPIOB, GPIO_PIN_6,   // IN3: PB6
	GPIOB, GPIO_PIN_7    // IN4: PB7
);

#if (_USE_STEP_MODE == _STEP_MODE_HALF)
static const uint8_t step_table[8][4] = {//얘도 풀스탭과 마찬가지로 72°를 돎(step angle이 18°를 반으로 쪼갠 거임)
	{1,0,1,0},
	{0,0,1,0},
	{0,1,1,0},
	{0,1,0,0},
	{0,1,0,1},
	{0,0,0,1},
	{1,0,0,1},
	{1,0,0,0}
};
#elif(_USE_STEP_MODE == _STEP_MODE_FULL)

static const uint8_t step_table[4][4] = {//72°를 돎(step angle이 18°라서)
	{1,0,1,0},  // A+ & B+
	{0,1,1,0},  // A- & B+
	{0,1,0,1},  // A- & B-
	{1,0,0,1},  // A+ & B-
};

#elif(_USE_STEP_MODE == _STEP_MODE_MICRO)
#define STEP_TABLE_SIZE		32

//sin table
//360도를 32스텝으로 쪼갰을 때 11.25가 나오는데 11.25도의 간격을 pwm으로 표현하면 이렇게 나옴
//여기서 말하는 360은 전류 벡터의 회전 경로가 360도라는 거고 모터는 동일하게 72도를 기준으로 잡음
const uint8_t step_table[32] = {			//sin(degree) -> pwm
  128, 152, 176, 198, 218, 234, 245, 253,
  255, 253, 245, 234, 218, 198, 176, 152,
  128, 103,  79,  57,  37,  21,  10,   2,
    0,   2,  10,  21,  37,  57,  79, 103
};
#endif


static uint8_t step_idx_left = 0;
static uint8_t step_idx_right = 0;
static uint32_t prev_us_left = 0;
static uint32_t prev_us_right = 0;

static void apply_step(StepMotor *m)
{
#if (_USE_STEP_MODE == _STEP_MODE_MICRO)
//	uint8_t now = __HAL_TIM_GET_COUNTER(&htim2) & 0xFF;//(0~255로 제한)
//	now = (now % 255);
	uint8_t now = TIM2->CNT & 0xFF;

	m->vA = step_table[m->step_idx];
	m->vB = step_table[(m->step_idx + (STEP_TABLE_SIZE >> 2)) & STEP_MASK]; //(STEP_MASK >> 2) == 8 == 90°(difference sin with cos)
	//sin파와 cos파의 위상 차가 90도가 나니까 +8을 한 거임 +8은 32를 360도로 치환했을 때 90도를 의미함

	if (now < m->vA)        	m->in1_port->BSRR = m->in1_pin;
	else                 		m->in1_port->BRR  = m->in1_pin;

	if (now < (255 - m->vA)) 	m->in2_port->BSRR = m->in2_pin;
	else                  		m->in2_port->BRR  = m->in2_pin;

	if (now < m->vB)         	m->in3_port->BSRR = m->in3_pin;
	else                  		m->in3_port->BRR  = m->in3_pin;

	if (now < (255 - m->vB)) 	m->in4_port->BSRR = m->in4_pin;
	else                 		m->in4_port->BRR  = m->in4_pin;
#else
  HAL_GPIO_WritePin(m->in1_port, m->in1_pin, step_table[m->step_idx][0]);
  HAL_GPIO_WritePin(m->in2_port, m->in2_pin, step_table[m->step_idx][1]);
  HAL_GPIO_WritePin(m->in3_port, m->in3_pin, step_table[m->step_idx][2]);
  HAL_GPIO_WritePin(m->in4_port, m->in4_pin, step_table[m->step_idx][3]);
#endif
}

void apply_test(uint8_t side)
{
	if(side == LEFT)
	{
		uint32_t now = TIM2->CNT;
		uint8_t pwm = now & 0xFF;
		uint8_t vA	= step_table[step_idx_left];
		uint8_t vB	= step_table[(step_idx_left + (STEP_TABLE_SIZE >> 2)) & STEP_MASK];


		if (pwm < vA)       L_IN1_PORT->BSRR = L_IN1_PIN;
		else                L_IN1_PORT->BRR  = L_IN1_PIN;

		// A- (IN2)
		if (pwm < (255 - vA)) L_IN2_PORT->BSRR = L_IN2_PIN;
		else                  L_IN2_PORT->BRR  = L_IN2_PIN;

		// B+ (IN3)
		if (pwm < vB)       L_IN3_PORT->BSRR = L_IN3_PIN;
		else                L_IN3_PORT->BRR  = L_IN3_PIN;

		// B- (IN4)
		if (pwm < (255 - vB)) L_IN4_PORT->BSRR = L_IN4_PIN;
		else                  L_IN4_PORT->BRR  = L_IN4_PIN;

		if(now - prev_us_left < 1500)
		{
			return;

		}
		prev_us_left = now;
		step_idx_left = (step_idx_left - 1) & STEP_MASK;

	}
	else if(side == RIGHT)
	{
		uint32_t now = TIM2->CNT;
		uint8_t pwm = now & 0xFF;
		uint8_t vA	= step_table[step_idx_right];
		uint8_t vB	= step_table[(step_idx_right + (STEP_TABLE_SIZE >> 2)) & STEP_MASK];

		if (pwm < vA)       R_IN1_PORT->BSRR = R_IN1_PIN;
		else                R_IN1_PORT->BRR  = R_IN1_PIN;

		// A- (IN2)
		if (pwm < (255 - vA)) R_IN2_PORT->BSRR = R_IN2_PIN;
		else                  R_IN2_PORT->BRR  = R_IN2_PIN;

		// B+ (IN3)
		if (pwm < vB)       R_IN3_PORT->BSRR = R_IN3_PIN;
		else                R_IN3_PORT->BRR  = R_IN3_PIN;

		// B- (IN4)
		if (pwm < (255 - vB)) R_IN4_PORT->BSRR = R_IN4_PIN;
		else                  R_IN4_PORT->BRR  = R_IN4_PIN;

		if(now - prev_us_right < 1500)
		{
			return;

		}
		prev_us_right = now;
		step_idx_right = (step_idx_right + 1) & STEP_MASK;
	}


}

void step_init(StepMotor *m)
{
  m->step_idx = 0;
  m->prev_time_us = 0;
  m->dir = (m == &step_motor_left) ? LEFT : RIGHT;

  m->forward = (m == &step_motor_left) ? step_forward : step_reverse;
  m->reverse = (m == &step_motor_left) ? step_reverse : step_forward;
  m->brake   = step_brake;
  m->slide   = step_slide;
}

void step_motor_init(void)
{
	step_motor_left.init(&step_motor_left);
	step_motor_right.init(&step_motor_right);

	step_drive(STOP);
}

void step_forward(StepMotor *m)
{
#if (_USE_STEP_MODE != _STEP_MODE_MICRO)
  apply_step(m);
  m->step_idx = (m->step_idx + 1) & STEP_MASK;
#else
  apply_step(m);
//  uint64_t now = __HAL_TIM_GET_COUNTER(&htim2);
  uint64_t now = TIM2->CNT;
//  uint64_t now = timer16_10us;
  idx_change = false;
  if(now - m->prev_time_us < m->period_us)
  {
	  return;
  }
  m->total_step++;
  idx_change = true;
  m->prev_time_us = now;
  m->step_idx = (m->step_idx - 1) & STEP_MASK;
#endif
}

void step_reverse(StepMotor *m)
{
#if (_USE_STEP_MODE != _STEP_MODE_MICRO)
  apply_step(m);
  m->step_idx = (m->step_idx - 1) & STEP_MASK;
#else
  apply_step(m);
//  uint64_t now = __HAL_TIM_GET_COUNTER(&htim2);
  uint64_t now = TIM2->CNT;
//  uint64_t now = timer16_10us;
  idx_change = false;
  if(now - m->prev_time_us < m->period_us)
  {
	return;
  }
  m->total_step++;
  idx_change = true;
  m->prev_time_us = now;
  m->step_idx = (m->step_idx + 1) & STEP_MASK;
#endif
}

void step_brake(StepMotor *m)
{
  HAL_GPIO_WritePin(m->in1_port, m->in1_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(m->in2_port, m->in2_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(m->in3_port, m->in3_pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(m->in4_port, m->in4_pin, GPIO_PIN_SET);
}

void step_slide(StepMotor *m)
{
  HAL_GPIO_WritePin(m->in1_port, m->in1_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(m->in2_port, m->in2_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(m->in3_port, m->in3_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(m->in4_port, m->in4_pin, GPIO_PIN_RESET);
}

uint32_t rpm_to_period(uint16_t rpm)
{
  if (rpm == 0) rpm = 1;
  if (rpm > SAFE_MAX_RPM) rpm = SAFE_MAX_RPM;
  return 60000000UL / (rpm * STEP_PER_REV);
}

uint32_t pwm_to_rpm(uint8_t pwm)
{
  if (pwm > MAX_SPEED) pwm = MAX_SPEED;
  return (uint32_t)pwm * SAFE_MAX_RPM / MAX_SPEED;
}

void step_operate(StepMotor *m, uint8_t speed, uint8_t dir)
{
  if (speed == 0)
  {
    m->brake(m);
    return;
  }

  m->period_us = rpm_to_period(pwm_to_rpm(speed));
  uint32_t now = __HAL_TIM_GET_COUNTER(&htim2);  // must be 1us timer

  if ((now - m->prev_time_us) >= m->period_us)
  {
    m->prev_time_us = now;
    (dir == FORWARD ? m->forward : m->reverse)(m);
  }
}



void step_idx_init(void)
{
	step_motor_left.step_idx = 0;
	step_motor_right.step_idx = 0;
}

uint32_t get_current_steps(void)
{
	uint32_t total_step = (step_motor_left.total_step + step_motor_right.total_step) / 2;
	return total_step;
}

void total_step_init(void)
{
	step_motor_left.total_step = 0;
	step_motor_right.total_step = 0;
}

void step_drive(StepOperation op)
{
	if(op == FORWARD)
	{
		step_motor_left.forward(&step_motor_left);
		step_motor_right.forward(&step_motor_right);
	}
	else if(op == REVERSE)
	{
		step_motor_left.reverse(&step_motor_left);
		step_motor_right.reverse(&step_motor_right);
	}
	else if(op == TURN_LEFT)
	{
		step_motor_left.reverse(&step_motor_left);
		step_motor_right.forward(&step_motor_right);
	}
	else if(op == TURN_RIGHT)
	{
		step_motor_left.forward(&step_motor_left);
		step_motor_right.reverse(&step_motor_right);
	}
	else if(op == STOP)
	{
		step_motor_left.brake(&step_motor_left);
		step_motor_right.brake(&step_motor_right);
	}
}

void step_drive_lightweight(void)
{
	uint64_t now = __HAL_TIM_GET_COUNTER(&htim2);

	apply_step(&step_motor_left);
	apply_step(&step_motor_right);

	if(now - step_motor_left.prev_time_us < 1500)
	{
		return;
  	}
	step_motor_left.prev_time_us = now;

	step_motor_left.step_idx = (step_motor_left.step_idx - 1) & STEP_MASK;
	step_motor_right.step_idx = (step_motor_right.step_idx + 1) & STEP_MASK;
}

void step_stop(void)
{
	step_motor_left.brake(&step_motor_left);
	step_motor_right.brake(&step_motor_right);
}

void step_set_period(uint16_t left_period, uint16_t right_period)
{
	step_motor_left.period_us = left_period;
	step_motor_right.period_us = right_period;
//	uart_printf("left_period: %d | right_period: %d\r\n", step_motor_left.period_us, step_motor_right.period_us);
}

void step_drive_ratio(uint16_t left_speed, uint16_t right_speed)  // 비율 기반 회전 제어
{
	step_set_period(left_speed, right_speed);
}

StepOperation mode_to_step(color_mode_t mode)
{
	switch (mode)
	{
		case MODE_FORWARD:
		case MODE_FAST_FORWARD:
		case MODE_SLOW_FORWARD:
		case MODE_LONG_FORWARD:
			return FORWARD;

		case MODE_BACKWARD:
		case MODE_FAST_BACKWARD:
		case MODE_SLOW_BACKWARD:
			return REVERSE;

		case MODE_LEFT:
			return TURN_LEFT;

		case MODE_RIGHT:
			return TURN_RIGHT;

		default:
			return NONE;
	}
}

uint16_t mode_to_step_count(color_mode_t mode)
{
	switch (mode)
	{
		case MODE_FORWARD:
		case MODE_BACKWARD:
		case MODE_FAST_FORWARD:
		case MODE_SLOW_FORWARD:
			return 1000;
		case MODE_FAST_BACKWARD:
		case MODE_SLOW_BACKWARD:
			return 850;

		case MODE_LEFT:
		case MODE_RIGHT:
			return 390;

		case MODE_LONG_FORWARD:
			return 1900;

		case MODE_LINE_TRACE:
			return 30000;

		default:
			return 0;
	}
}

uint16_t mode_to_left_period(color_mode_t mode)
{
	switch (mode)
	{
		case MODE_FAST_FORWARD:   return 2000;
		case MODE_SLOW_FORWARD:   return 700;
		case MODE_FAST_BACKWARD:  return 1500;
		case MODE_SLOW_BACKWARD:  return 1000;
		default:                  return 1000; // 기본값
	}
}

uint16_t mode_to_right_period(color_mode_t mode)
{
	switch (mode)
	{
		case MODE_FAST_FORWARD:   return 700;
		case MODE_SLOW_FORWARD:   return 2000;
		case MODE_FAST_BACKWARD:  return 1000;
		case MODE_SLOW_BACKWARD:  return 1500;
		default:                  return 1000; // 기본값
	}
}
