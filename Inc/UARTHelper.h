#ifndef __UARTHELPER_H__
#define __UARTHELPER_H__

#include "stm32f4xx_hal.h"

typedef struct {
	UART_HandleTypeDef uartHandler;
	uint8_t rxBuffer;
	int commandCount;
	int bufferCounter;
	char buffer[100];
} UartInterface;


void startUARTReceiveDMA(UartInterface * interface);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void MX_UART_Init(UART_HandleTypeDef * uartHandler, USART_TypeDef * uartInstance, int baudRate);

typedef struct {
	UartInterface * mainBoard;
	UartInterface * testBoard0MCU0;
	UartInterface * testBoard0MCU1;
	UartInterface * testBoard1MCU0;
	UartInterface * testBoard1MCU1;
} NamedUARTInterface;


#endif
