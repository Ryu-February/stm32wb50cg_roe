/*
 * ap.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fbcks
 */


#include "ap.h"


volatile uint8_t detected_color = COLOR_BLACK;
extern volatile bool pb0_pressed;
extern volatile bool check_color;
extern volatile bool color_calibration;
extern volatile uint32_t timer17_ms;
extern volatile uint32_t pb0_pressed_ms;

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
	load_color_reference_table();
}


void ap_main(void)
{
//	uint32_t prev_time = 0;


	while(1)
	{
//		if(millis() - prev_time > 500)
//		{
//			uint8_t adc_val = ir_read_adc();
//			uart_printf("ir_val: %d\r\n", adc_val);
//		}
		uart_printf("pb0_pressed_ms: %d\r\n", pb0_pressed_ms);

//		if(check_color == true)
//		{
//			uint8_t left_color 	= classify_color_side(BH1745_ADDR_LEFT);
//			uint8_t right_color = classify_color_side(BH1745_ADDR_RIGHT);
//
//			if(left_color == right_color)
//			{
//				detected_color = left_color;
//			}
//			uart_printf("[left_color ]: %s\r\n", color_to_string(left_color));
//			uart_printf("[right_color]: %s\r\n", color_to_string(right_color));
//
//			check_color = false;
//		}

		if(color_calibration && check_color)
		{
			static uint8_t color_seq = 0;

			uart_printf("-------------COLOR SETTING-------------\r\n");
			uart_printf("color set: [%s]\r\n", color_to_string(color_seq));


			bh1745_color_data_t left  = bh1745_read_rgbc(BH1745_ADDR_LEFT);
			bh1745_color_data_t right = bh1745_read_rgbc(BH1745_ADDR_RIGHT);

			uart_printf("[LEFT]  R:%u G:%u B:%u C:%u\r\n",
					left.red, left.green, left.blue, left.clear);

			uart_printf("[RIGHT] R:%u G:%u B:%u C:%u\r\n",
					right.red, right.green, right.blue, right.clear);

			//save_color에서 RWX permission warning 뜸
			save_color_reference(BH1745_ADDR_LEFT, color_seq, left.red, left.green, left.blue);
			save_color_reference(BH1745_ADDR_RIGHT, color_seq, right.red, right.green, right.blue);

			uart_printf("--------------------------------\r\n");
			if(color_seq++ > COLOR_GRAY)
			{
				color_calibration = false;
			}
		}
	}
}
