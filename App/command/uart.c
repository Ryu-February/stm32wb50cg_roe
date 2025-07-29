/*
 * uart.c
 *
 *  Created on: Jul 28, 2025
 *      Author: fbcks
 */


#include "uart.h"

extern UART_HandleTypeDef huart1;



void uart_init(void)
{
	HAL_UART_Init(&huart1);
}


void uart_printf(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}
