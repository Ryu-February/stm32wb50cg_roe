/*
 * line_tracing.h
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */

#ifndef CONTROL_LINE_TRACING_H_
#define CONTROL_LINE_TRACING_H_



#include "line_tracing.h"
#include "color.h"


typedef enum
{
	ON_LINE,
	LEFT_OFF,
	RIGHT_OFF,
	LOST
}LineState;

void line_tracing_fsm(void);
void line_tracing_pid(void);
bh1745_color_data_t line_tracing_read_rgb(uint8_t color_addr);


#endif /* CONTROL_LINE_TRACING_H_ */
