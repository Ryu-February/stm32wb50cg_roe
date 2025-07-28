/*
 * uart.h
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */

#ifndef COMMAND_UART_H_
#define COMMAND_UART_H_


#include "def.h"


void uart_init(void);
void uart_printf(const char *fmt, ...);


#endif /* COMMAND_UART_H_ */
