#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "UARTHelper.h"
#include <stdbool.h>

#define MAIN_BOARD_LOST_THRESHOLD		1
#define DEBUG												1
#define DEBUG_UART									(&uartInterfaces[0])
#define BAUD_RATE										9600

typedef struct {
	UartInterface * mainBoard;
	UartInterface * testBoard0;
	UartInterface * testBoard1;
} NamedUARTInterface;

typedef struct {
	bool isInserted;
	char uuid[36];
	bool isHVOK;
} TestBoardStatus;

extern UartInterface uartInterfaces[8];
extern NamedUARTInterface namedUARTInterface;
extern uint32_t lastMainBoardResponseTick;
extern TestBoardStatus testBoardStatus[2];
extern bool isMainBoardConnected;
extern void restore15VChannels(void);
#endif
