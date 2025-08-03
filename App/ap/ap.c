/*
 * ap.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */


#include "ap.h"


volatile uint8_t detected_color = COLOR_BLACK;
volatile bool color_calibration = false;
extern volatile bool check_color;
extern volatile bool mode_update;
extern volatile bool rgb_update;
extern volatile bool step_update;

static bool init_printed = false;
static uint8_t color_seq = 0;

static void ap_task_color_calibration(void);
static void ap_task_color_detection(void);


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
	step_set_period(1500, 1500);
	load_color_reference_table();
	debug_print_color_reference_table();
}


void ap_main(void)
{
	while(1)
	{
		if(!color_calibration && input_is_long_pressed(INPUT_MODE))
		{
			color_calibration = true;
			color_seq = 0;
			init_printed = false;
			uart_printf("[INFO] Entering color calibration mode...\r\n");
		}

		if (color_calibration)
		{
			ap_task_color_calibration();
		}
		else
		{
			ap_task_color_detection();
		}
//		uart_printf("hi\r\n");
//		uart_printf("cheeze\r\n");
//		uart_printf("kimchi\r\n");

		if(rgb_update)
		{
			rgb_set_color(detected_color);
			rgb_update = false;
		}

		if(step_update)
		{
			switch (detected_color)
			{
				case COLOR_RED :
		//			step_drive(FORWARD);
					apply_test(LEFT);
					apply_test(RIGHT);
					break;
				case COLOR_ORANGE :
					step_drive(REVERSE);
					break;
				case COLOR_YELLOW :
					step_drive(TURN_LEFT);
					break;
				case COLOR_GREEN :
					step_drive(TURN_RIGHT);
					break;
			}
			step_update = false;
		}
	}
}



static void ap_task_color_calibration(void)
{
	if(!check_color) return;

	if (!init_printed)
	{
		uart_printf("-------------COLOR SETTING-------------\r\n");
		init_printed = true;
		flash_erase_color_table(BH1745_ADDR_LEFT);
		flash_erase_color_table(BH1745_ADDR_RIGHT);
	}

	if (input_is_short_pressed(INPUT_MODE))
	{
		uart_printf("color set: [%s]\r\n", color_to_string(color_seq));

		bh1745_color_data_t left  = bh1745_read_rgbc(BH1745_ADDR_LEFT);
		bh1745_color_data_t right = bh1745_read_rgbc(BH1745_ADDR_RIGHT);

		uart_printf("[LEFT]  R:%u G:%u B:%u C:%u\r\n",
					left.red, left.green, left.blue, left.clear);

		uart_printf("[RIGHT] R:%u G:%u B:%u C:%u\r\n",
					right.red, right.green, right.blue, right.clear);

		save_color_reference(BH1745_ADDR_LEFT,  color_seq, left.red, left.green, left.blue);
		save_color_reference(BH1745_ADDR_RIGHT, color_seq, right.red, right.green, right.blue);

		uart_printf("--------------------------------\r\n");

		if (++color_seq > COLOR_GRAY)
		{
			color_calibration = false;
			init_printed = false;
			color_seq = 0;
			uart_printf("-------color set finished-------\r\n");
			uart_printf("--------------------------------\r\n");
			load_color_reference_table();
			debug_print_color_reference_table();
		}
	}
}


// -------------------- 일반 색상 인식 루틴 --------------------
static void ap_task_color_detection(void)
{
	if (!check_color) return;

	uint8_t left  = classify_color_side(BH1745_ADDR_LEFT);
	uint8_t right = classify_color_side(BH1745_ADDR_RIGHT);

	if (left == right)
	{
		detected_color = left;
		mode_update = true;
		uart_printf("cur_detected color: %s\r\n", color_to_string(left));
	}
	else
	{
		detected_color = COLOR_BLACK;
		uart_printf("The colors on both sides do not match!!\r\n");
		uart_printf("[LEFT]: %s | [RIGHT]: %s\r\n", color_to_string(left), color_to_string(right));
	}

	check_color = false;
}
