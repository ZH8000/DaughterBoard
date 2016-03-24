#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "UARTHelper.h"
#include <stdbool.h>

#define MAIN_BOARD_LOST_THRESHOLD		3
#define DEBUG												1

typedef struct {
	UartInterface * mainBoard;
	UartInterface * testBoard0;
	UartInterface * testBoard1;
} NamedUARTInterface;

typedef struct {
	bool isInserted;
	char uuid[36];
} TestBoardStatus;

extern UartInterface uartInterfaces[8];
extern NamedUARTInterface namedUARTInterface;
extern uint32_t lastMainBoardResponseTick;
extern TestBoardStatus testBoardStatus[2];
extern UartInterface * getUARTInterface(UART_HandleTypeDef *huart, int * whichUART);

#endif
