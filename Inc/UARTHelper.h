#ifndef __UARTHELPER_H__
#define __UARTHELPER_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>


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
void sendToUART(UartInterface * uartInterface, char * format, ...);
typedef struct {
	UartInterface * mainBoard;
	UartInterface * testBoard0;
	UartInterface * testBoard1;
} NamedUARTInterface;
typedef struct {
	bool isInserted;
	char uuid[36];
} TestBoardStatus;


#endif
