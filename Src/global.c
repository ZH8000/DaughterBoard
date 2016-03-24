
/*
UartInterface uartInterfaces[8];
NamedUARTInterface namedUARTInterface;
uint32_t lastMainBoardResponseTick = 0;
#define MAIN_BOARD_LOST_THRESHOLD		3
#define DEBUG	



TestBoardStatus testBoardStatus[2];
*/

#include "global.h"

UartInterface uartInterfaces[8];
NamedUARTInterface namedUARTInterface;
TestBoardStatus testBoardStatus[2];

uint32_t lastMainBoardResponseTick = 0;


UartInterface * getUARTInterface(UART_HandleTypeDef *huart, int * whichUART) {
	if (huart == &(uartInterfaces[0].uartHandler)) {
		*whichUART = 1;
		return &(uartInterfaces[0]);
	}
	else if (huart == &(uartInterfaces[1].uartHandler)) {
		*whichUART = 2;
		return &(uartInterfaces[1]);
	}
	else if (huart == &(uartInterfaces[2].uartHandler)) {
		*whichUART = 3;
		return &(uartInterfaces[2]);
	}
	else if (huart == &(uartInterfaces[3].uartHandler)) {
		*whichUART = 4;
		return &(uartInterfaces[3]);
	}
	else if (huart == &(uartInterfaces[4].uartHandler)) {
		*whichUART = 5;
		return &(uartInterfaces[4]);
	}
	else if (huart == &(uartInterfaces[5].uartHandler)) {
		*whichUART = 6;
		return &(uartInterfaces[5]);
	}
	else if (huart == &(uartInterfaces[6].uartHandler)) {
		*whichUART = 7;		
		return &(uartInterfaces[6]);
	}
	else if (huart == &(uartInterfaces[7].uartHandler)) {
		*whichUART = 8;				
		return &(uartInterfaces[7]);
	} else {
		*whichUART = -1;						
		return NULL;
	}
}
