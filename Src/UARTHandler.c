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
int count2 = 0;
void processMainBoardCommand(char * command, UartInterface * sender) {
	
	//debugMessage("MBCommand: %s, isCorrectCommandFromMB: %d\n", command, isCorrectCommandFromMB(command));
	if (isCorrectCommandFromMB(command)) {
		debugMessage("command[%d]: %s\n", count2, command);
		UartInterface * uartInterface;
		
		if (command[1] == '0') {
			uartInterface = namedUARTInterface.testBoard0;
		} else if (command[1] == '1') {
			uartInterface = namedUARTInterface.testBoard1;
		}
		
		char * subCommand = command + 2;
		HAL_StatusTypeDef status = sendToUART(uartInterface, "%s\n", subCommand);
		debugMessage("command=%s, status=%d\n", subCommand, status);
		count2++;
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
	debugMessage("response[%d]: %s\n", count, response);
	sendToUART(namedUARTInterface.mainBoard, "#%d%s\n", whichTestBoard, response);		
	if (response[0] == '#' && response[1] == 'f' && response[2] == '#' && strlen(response) == 40) {
		strncpy(testBoardStatus[whichTestBoard].uuid, response + 3, 36);		
	} else {
		count++;
	}
}

void uartReceiverCallback(UartInterface * uartInterface, char * content) {
	bool isMainBoard = uartInterface == namedUARTInterface.mainBoard;
	bool isTestBoard = (uartInterface == namedUARTInterface.testBoard0) || (uartInterface == namedUARTInterface.testBoard1);
	bool isCommand = strlen(content) > 1 && content[0] == '$';
	bool isResponse = strlen(content) > 1 && content[0] == '#';
						
	if (isMainBoard && isCommand) {
		processMainBoardCommand(content, uartInterface);
	} else if (isMainBoard && isResponse) {
		processMainBoardResponse(content, uartInterface);
	} else if (isTestBoard && isResponse) {
		processTestBoardResponse(content, uartInterface);				
	}
	
}
