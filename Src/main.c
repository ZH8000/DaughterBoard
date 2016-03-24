/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <string.h>
#include "UARTHelper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "global.h"


/* Private variables ---------------------------------------------------------*/
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart5_rx;
DMA_HandleTypeDef hdma_uart7_rx;
DMA_HandleTypeDef hdma_uart8_rx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart6_rx;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void initUART(void);





void sendPingToMainBoard() {
	sendToUART(namedUARTInterface.mainBoard, "$PING$%d$%s$\n", testBoardStatus[0].isInserted, testBoardStatus[0].uuid);
	sendToUART(namedUARTInterface.mainBoard, "$PING$%d$%s$\n", testBoardStatus[1].isInserted, testBoardStatus[1].uuid);
}


bool isMainBoardLost() {
	int32_t currentTick = HAL_GetTick();
	int32_t duration = currentTick - lastMainBoardResponseTick;
	
	// 如果 duration 大於 currentTick，代表 HAL_GetTick() 發生了 overflow，超過了
	// 32bit 無號整數的最大值，計算的方法為先算出 32bit 無號整數的最大值 4294967295
	// 離 lastMainBoardResponseTick 有多遠，再加上從 overflow 發生的時間點到現在經
	// 過了多少個 Tick。
	if (duration > currentTick) {
		duration = 4294967295 - currentTick + duration;
	}
	
	debugMessage(
		"currentTick(%u) - lastTick(%u) = %d / %d\n", 
		currentTick, 
		lastMainBoardResponseTick, 
		duration, 
		MAIN_BOARD_LOST_THRESHOLD * 60 * 1000
	);
	
	return duration > MAIN_BOARD_LOST_THRESHOLD * 60 * 1000;
}

void shutdownAllChannel() {
	
	if (isMainBoardConnected) {
		debugMessage("SHUT DOWN ALL CHANNEL\n");
		HAL_GPIO_WritePin(GPIOB, TB0_LCR_Pin|TB1_LCR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, TB0_HV_Pin|TB1_HV_Pin|TB0_15V_Pin|TB1_15V_Pin|TB0_LC_Pin|TB1_LC_Pin, GPIO_PIN_RESET);
	}
}

void initTestBoardStatus() {
	testBoardStatus[0].isInserted = false;
	testBoardStatus[1].isInserted = false;
	testBoardStatus[0].isHVOK = false;
	testBoardStatus[1].isHVOK = false;
	memset(testBoardStatus[0].uuid, 0, 36);
	memset(testBoardStatus[1].uuid, 0, 36);
}

int isCorrectCommandFromMB(char * command) {
	return strlen(command) == 7 &&
				 (*(command+0) == '$') &&
				 (*(command+1) == '0' || *(command+1) == '1') &&
				 (*(command+2) == '$') &&
				 (*(command+4) == '$') &&
				 (*(command+6) == '$');
}

void processMainBoardCommand(char * command, UartInterface * sender) {
	
	debugMessage("MBCommand: %s, isCorrectCommandFromMB: %d\n", command, isCorrectCommandFromMB(command));
	if (isCorrectCommandFromMB(command)) {
		UartInterface * uartInterface;
		
		if (command[1] == '0') {
			uartInterface = namedUARTInterface.testBoard0;
		} else if (command[1] == '1') {
			uartInterface = namedUARTInterface.testBoard1;
		}
		
		char * subCommand = command + 2;
		sendToUART(uartInterface, subCommand);
		sendToUART(uartInterface, "\n");
	}
	
}

void processMainBoardResponse(char * response, UartInterface * sender) {
	debugMessage("GotResponseFromMB: %s\n", response);
	
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

void processTestBoardResponse(char * response, UartInterface * sender) {
	int whichTestBoard = getWhichTestBoard(sender);
	sendToUART(namedUARTInterface.mainBoard, "#%d%s\n", whichTestBoard, response);		
	
	if (response[0] == '#' && response[1] == 'f' && response[2] == '#' && strlen(response) == 40) {
		strncpy(testBoardStatus[whichTestBoard].uuid, response + 3, 36);		
	}
}

void processUARTContent() {
	
	for (int i = 0; i < 8; i++) {
		UartInterface * uartInterface = &uartInterfaces[i];
		if (uartInterface->shouldProcessContent) {
			uartInterface->shouldProcessContent = false;
			char * buffer = uartInterface->command;
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
		}

	}
}

void initUART(void) {
	MX_UART_Init(&(uartInterfaces[0].uartHandler), USART1, 115200);
	MX_UART_Init(&(uartInterfaces[1].uartHandler), USART2, 115200);
	MX_UART_Init(&(uartInterfaces[2].uartHandler), USART3, 115200);
	MX_UART_Init(&(uartInterfaces[3].uartHandler), UART4,  115200);
	MX_UART_Init(&(uartInterfaces[4].uartHandler), UART5,  115200);
	MX_UART_Init(&(uartInterfaces[5].uartHandler), USART6, 115200);
	MX_UART_Init(&(uartInterfaces[6].uartHandler), UART7,  115200);
	MX_UART_Init(&(uartInterfaces[7].uartHandler), UART8,  115200);
	
	// Define Named UART
	//namedUARTInterface.mainBoard = &uartInterfaces[1];
	namedUARTInterface.mainBoard = &uartInterfaces[0];
	//namedUARTInterface.testBoard0MCU0 = &uartInterfaces[3];
	namedUARTInterface.testBoard0 = &uartInterfaces[2];
	namedUARTInterface.testBoard1 = &uartInterfaces[7];
}


void checkTestBoardStatus(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int whichTestBoard) {
	GPIO_PinState newState = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
	
	if (newState != testBoardStatus[whichTestBoard].isInserted) {
		
		testBoardStatus[whichTestBoard].isInserted = newState;

		if (newState == GPIO_PIN_SET) {
			debugMessage("TestBoard[%d] plugged...\n", whichTestBoard);
			if (whichTestBoard == 0) {
				sendToUART(namedUARTInterface.testBoard0, "$f$$$\n");
			} else if (whichTestBoard == 1) {
				sendToUART(namedUARTInterface.testBoard1, "$f$$$\n");				
			}
		} else if (newState == GPIO_PIN_RESET) {
			debugMessage("TestBoard[%d] unplugged...\n", whichTestBoard);
			memset(testBoardStatus[whichTestBoard].uuid, 0, 36);
			if (whichTestBoard == 0) {
				sendToUART(namedUARTInterface.mainBoard, "#0#g###\n");
			} else if (whichTestBoard == 1) {
				sendToUART(namedUARTInterface.mainBoard, "#1#g###\n");				
			}
		}
	}
}

int main(void)
{


  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
	initTestBoardStatus();
	initUART();
	startUARTReceiveDMA(&uartInterfaces[0]);
	startUARTReceiveDMA(&uartInterfaces[1]);
	startUARTReceiveDMA(&uartInterfaces[2]);
	startUARTReceiveDMA(&uartInterfaces[3]);
	startUARTReceiveDMA(&uartInterfaces[4]);
	startUARTReceiveDMA(&uartInterfaces[5]);
	startUARTReceiveDMA(&uartInterfaces[6]);
	startUARTReceiveDMA(&uartInterfaces[7]);
	/*	Infinite loop */
	HAL_Delay(1000);
	uint32_t lastTime = 0;
	
  while (1)
  {
		processUARTContent();

		uint32_t tick = HAL_GetTick();
		uint32_t round = tick / 1000;
		
		if (lastTime != round) {
			sendPingToMainBoard();
			if (isMainBoardLost()) {
				//shutdownAllChannel();
				isMainBoardConnected = false;			
			}
			checkTestBoardStatus(TB0_DETECT_GPIO_Port, TB0_DETECT_Pin, 0);
			checkTestBoardStatus(TB1_DETECT_GPIO_Port, TB1_DETECT_Pin, 1);	
		}
		lastTime = round;		
  }

}


	
/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}



/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __DMA1_CLK_ENABLE();
  __DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOA_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TB0_LCR_Pin|TB1_LCR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, TB0_HV_Pin|TB1_HV_Pin|TB0_15V_Pin|TB1_15V_Pin 
                          |TB0_LC_Pin|TB1_LC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TB0_LCR_Pin TB1_LCR_Pin */
  GPIO_InitStruct.Pin = TB0_LCR_Pin|TB1_LCR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : TB0_DETECT_Pin TB1_DETECT_Pin ADC_CLK_Pin ADC_DATAIN_Pin */
  GPIO_InitStruct.Pin = TB0_DETECT_Pin|TB1_DETECT_Pin|ADC_CLK_Pin|ADC_DATAIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : TB0_HV_Pin TB1_HV_Pin TB0_15V_Pin TB1_15V_Pin 
                           TB0_LC_Pin TB1_LC_Pin */
  GPIO_InitStruct.Pin = TB0_HV_Pin|TB1_HV_Pin|TB0_15V_Pin|TB1_15V_Pin 
                          |TB0_LC_Pin|TB1_LC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\n", file, line) */
  /* USER CODE END 6 */

}

#endif
