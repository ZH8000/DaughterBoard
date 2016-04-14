#include <string.h>
#include "global.h"
#include "UARTHelper.h"
#include "UARTHandler.h"


int isCorrectCommandFromMB(char * command) {
	return strlen(command) == 7 &&
				 (*(command+0) == '$') &&
				 (*(command+1) == '0' || *(command+1) == '1') &&
				 (*(command+2) == '$') &&
				 (*(command+4) == '$') &&
				 (*(command+6) == '$');
}
void processMainBoardCommand(char * command, UartInterface * sender) {
	
	//debugMessage("MBCommand: %s, isCorrectCommandFromMB: %d\n", command, isCorrectCommandFromMB(command));
	if (isCorrectCommandFromMB(command)) {
		
		int whichTB = -1;		
		UartInterface * uartInterface;
		
		if (command[1] == '0') {
			uartInterface = namedUARTInterface.testBoard0;
			whichTB = 0;
		} else if (command[1] == '1') {
			uartInterface = namedUARTInterface.testBoard1;
			whichTB = 1;
		}
		
		char * subCommand = command + 2;
		debugMessage("subCommand:%s\n", subCommand);
		
		char commandCode = subCommand[1];
		
		GPIO_PinState newPinState = GPIO_PIN_RESET;
		uint16_t lcrPin = 0;
		uint16_t lcPin = 0;
		uint16_t hvPin = 0;
		
		switch(commandCode) {
			case 'a':
				lcrPin = whichTB == 0 ? TB0_LCR_Pin : TB1_LCR_Pin;
				newPinState = subCommand[3] == '0' ? GPIO_PIN_RESET : GPIO_PIN_SET;
				debugMessage("PULL LCR PIN[%d] to: %d\n", lcrPin, newPinState);
				HAL_GPIO_WritePin(TB0_LCR_GPIO_Port, lcrPin, newPinState);
				break;
			case 'b':
				lcPin = whichTB == 0 ? TB0_LC_Pin : TB1_LC_Pin;
				newPinState = subCommand[3] == '0' ? GPIO_PIN_RESET : GPIO_PIN_SET;
				debugMessage("PULL LC PIN[%d] to: %d\n", lcPin, newPinState);
				HAL_GPIO_WritePin(TB0_LC_GPIO_Port, lcPin, newPinState);				
				break;
			case 'c':
				hvPin = whichTB == 0 ? TB0_HV_Pin : TB1_HV_Pin;
				newPinState = subCommand[3] == '0' ? GPIO_PIN_RESET : GPIO_PIN_SET;
				debugMessage("PULL HV PIN[%d] to: %d\n", hvPin, newPinState);
				HAL_GPIO_WritePin(TB0_HV_GPIO_Port, hvPin, newPinState);				
				break;
		}
		
		sendToUART(uartInterface, "%s\n", subCommand);
	}
	
}

void processMainBoardResponse(char * response, UartInterface * sender) {
	//debugMessage("GotResponseFromMB: %s\n", response);
	
	if (strcmp(response, "#PONG#") == 0) {
		if (!isMainBoardConnected) {
			restore15VChannels();
		}
		isMainBoardConnected = true;
		lastMainBoardResponseTick = HAL_GetTick();
	}
}


int getWhichTestBoard(UartInterface * sender) {
	if (sender == namedUARTInterface.testBoard0) {
		return 0;
	} else if (sender == namedUARTInterface.testBoard1) {
		return 1;
	}
	return -1;
}

int count = 0;

void processTestBoardResponse(char * response, UartInterface * sender) {
	int whichTestBoard = getWhichTestBoard(sender);
	debugMessage("respond[%d]: %s\n", count, response);
	sendToUART(namedUARTInterface.mainBoard, "#%d%s\n", whichTestBoard, response);		
	if (response[0] == '#' && response[1] == 'f' && response[2] == '#' && strlen(response) == 40) {
		strncpy(testBoardStatus[whichTestBoard].uuid, response + 3, 36);		
		testBoardStatus[whichTestBoard].isInserted = 1;
		debugMessage("TestBoard[%d] plugged...\n", whichTestBoard);
		
	} else {
		count++;
	}
}

void uartReceiverCallback(UartInterface * uartInterface, char * content) {
	bool isMainBoard = uartInterface == namedUARTInterface.mainBoard;
	bool isTestBoard = (uartInterface == namedUARTInterface.testBoard0) || (uartInterface == namedUARTInterface.testBoard1);
	bool isCommand = strlen(content) > 1 && content[0] == '$';
	bool isResponse = strlen(content) > 1 && content[0] == '#';
	
	if (isTestBoard) {
		debugMessage("Received: %s\n", content);
	}
	if (isMainBoard && isCommand) {
		processMainBoardCommand(content, uartInterface);
	} else if (isMainBoard && isResponse) {
		processMainBoardResponse(content, uartInterface);
	} else if (isTestBoard && isResponse) {
		processTestBoardResponse(content, uartInterface);				
	}
	
}
