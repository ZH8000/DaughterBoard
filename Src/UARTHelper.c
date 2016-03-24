#include "UARTHelper.h"
#include <string.h>
#include <stdarg.h>
#include "global.h"

void debugMessage(char * format, ...) {
	#ifdef DEBUG
		char message[100] = {0};
		va_list argptr;
		va_start(argptr,format);
		vsnprintf(message, 100, format, argptr);
		va_end(argptr);
		HAL_UART_Transmit(&DEBUG_UART->uartHandler, (uint8_t *) message, strlen(message), 100);		
	#endif
}

void sendToUART(UartInterface * uartInterface, char * format, ...) {
	char message[100] = {0};
	va_list argptr;
	va_start(argptr,format);
	vsnprintf(message, 100, format, argptr);
	va_end(argptr);
	HAL_UART_Transmit(&uartInterface->uartHandler, (uint8_t *) message, strlen(message), 100);		
}


void startUARTReceiveDMA(UartInterface * interface) {
	HAL_UART_Receive_DMA(&(interface->uartHandler), &(interface->rxBuffer), 1);	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	__HAL_UART_FLUSH_DRREGISTER(huart);
	uint8_t receiveChar = *(huart->pRxBuffPtr);
	int whichUART;
	
	UartInterface * uartInterface = getUARTInterface(huart, &whichUART);
		
	if (uartInterface != NULL) {
		if (receiveChar == '\r') {
			// Ingore CR
		} else if (receiveChar == '\n') {
			
			/*
			char * buffer = uartInterface->buffer;
			bool isMainBoard = uartInterface == namedUARTInterface.mainBoard;
			bool isTestBoard = (uartInterface == namedUARTInterface.testBoard0) || (uartInterface == namedUARTInterface.testBoard1);
			bool isCommand = strlen(buffer) > 1 && buffer[0] == '$';
			bool isResponse = strlen(buffer) > 1 && buffer[0] == '#';
						
			if (isMainBoard && isCommand) {
				processMainBoardCommand(buffer, uartInterface);
			} else if (isMainBoard && isResponse) {
				processMainBoardResponse(buffer, uartInterface);
			} else if (isTestBoard && isResponse) {
				processTestBoardResponse(buffer, uartInterface);				
			}
			*/
			
			strncpy(uartInterface->command, uartInterface->buffer, 100);
			uartInterface->shouldProcessContent = true;
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
