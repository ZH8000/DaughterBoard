#include "UARTHelper.h"
#include <string.h>

extern UartInterface * getUARTInterface(UART_HandleTypeDef *huart, int * whichUART);
extern UartInterface uartInterfaces[8];
extern NamedUARTInterface namedUARTInterface;

void sendToUART(UartInterface * uartInterface, char * message) {
	HAL_UART_Transmit(&uartInterface->uartHandler, (uint8_t *) message, strlen(message), 100);	
}

int isCorrectCommandFromMB(char * command) {
	return strlen(command) == 9 &&
				 (*(command+0) == '$') &&
				 (*(command+1) == '0' || *(command+1) == '1') &&
				 (*(command+2) == '$') &&
				 (*(command+3) == '0' || *(command+1) == '1') &&
				 (*(command+4) == '$') &&
				 (*(command+6) == '$') &&
				 (*(command+8) == '$');
}

void processMainBoardCommand(char * command, UartInterface * sender) {
	
	if (isCorrectCommandFromMB(command)) {
		UartInterface * uartInterface;
		
		if (command[1] == '0' && command[3] == '0') {
			uartInterface = namedUARTInterface.testBoard0MCU0;
		} else if (command[1] == '0' && command[3] == '1') {
			uartInterface = namedUARTInterface.testBoard0MCU1;
		} else if (command[1] == '1' && command[2] == '0') {
			uartInterface = namedUARTInterface.testBoard1MCU0;
		}else if (command[1] == '1' && command[2] == '1') {
			uartInterface = namedUARTInterface.testBoard1MCU1;
		}
		
		char * subCommand = command + 4;
		sendToUART(uartInterface, subCommand);
		sendToUART(uartInterface, "\n");
	}
	
}

void getWhichTestBoard(UartInterface * sender, int * whichTestBoard, int * whichMCU) {
	if (sender == namedUARTInterface.testBoard0MCU0) {
		*whichTestBoard = 0;
		*whichMCU = 0;
	} else if (sender == namedUARTInterface.testBoard0MCU1) {
		*whichTestBoard = 0;
		*whichMCU = 1;		
	} else if (sender == namedUARTInterface.testBoard1MCU0) {
		*whichTestBoard = 1;
		*whichMCU = 0;		
	}	else if (sender == namedUARTInterface.testBoard1MCU1) {
		*whichTestBoard = 1;
		*whichMCU = 1;		
	}
}

void processTestBoardCommand(char * command, UartInterface * sender) {
	int whichTestBoard = -1;
	int whichMCU = -1;
	getWhichTestBoard(sender, &whichTestBoard, &whichMCU);
	char message[100] = {0};
	memset(message, 0, 100);
	sprintf(message, "#%d#%d%s\r\n", whichTestBoard, whichMCU, command);
	HAL_UART_Transmit(&namedUARTInterface.mainBoard->uartHandler, (uint8_t *) &message, strlen(message), 100);		
}

void startUARTReceiveDMA(UartInterface * interface) {
	HAL_UART_Receive_DMA(&(interface->uartHandler), &(interface->rxBuffer), 1);	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	//__HAL_UART_FLUSH_DRREGISTER(huart);
	uint8_t receiveChar = *(huart->pRxBuffPtr);
	int whichUART;
	
	UartInterface * uartInterface = getUARTInterface(huart, &whichUART);
	
	if (uartInterface != NULL) {
		if (receiveChar == '\r') {
			// Ingore CR
		} else if (receiveChar == '\n') {
			
			int isMainBoard = uartInterface == namedUARTInterface.mainBoard;
			int isTestBoard = 
				(uartInterface == namedUARTInterface.testBoard0MCU0) || 
				(uartInterface == namedUARTInterface.testBoard0MCU1) || 
				(uartInterface == namedUARTInterface.testBoard1MCU0) || 
				(uartInterface == namedUARTInterface.testBoard1MCU1);
			
			if (isMainBoard) {
				processMainBoardCommand(uartInterface->buffer, uartInterface);
			} else if (isTestBoard) {
				processTestBoardCommand(uartInterface->buffer, uartInterface);				
			}
			
			memset(uartInterface->buffer, 0, 100);
			uartInterface->bufferCounter = 0;
			uartInterface->commandCount++;
		} else {
			uartInterface->buffer[uartInterface->bufferCounter] = receiveChar;
			uartInterface->bufferCounter++;
		}
	}

	HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr, 1);	
}

void MX_UART_Init(UART_HandleTypeDef * uartHandler, USART_TypeDef * uartInstance, int baudRate)
{

  uartHandler->Instance = uartInstance;
  uartHandler->Init.BaudRate = baudRate;
  uartHandler->Init.WordLength = UART_WORDLENGTH_8B;
  uartHandler->Init.StopBits = UART_STOPBITS_1;
  uartHandler->Init.Parity = UART_PARITY_NONE;
  uartHandler->Init.Mode = UART_MODE_TX_RX;
  uartHandler->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandler->Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(uartHandler);
}
