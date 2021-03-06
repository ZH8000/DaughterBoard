/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "UARTHelper.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart5_rx;
extern DMA_HandleTypeDef hdma_uart7_rx;
extern DMA_HandleTypeDef hdma_uart8_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart8;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
extern UartInterface uartInterfaces[8];


/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
    /* USER CODE BEGIN SysTick_IRQn 1 */

    /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles DMA1 stream0 global interrupt.
 */
void DMA1_Stream0_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */

    /* USER CODE END DMA1_Stream0_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_uart5_rx);
    /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */

    /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream1 global interrupt.
 */
void DMA1_Stream1_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

    /* USER CODE END DMA1_Stream1_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_usart3_rx);
    /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

    /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream2 global interrupt.
 */
void DMA1_Stream2_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Stream2_IRQn 0 */

    /* USER CODE END DMA1_Stream2_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_uart4_rx);
    /* USER CODE BEGIN DMA1_Stream2_IRQn 1 */

    /* USER CODE END DMA1_Stream2_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream3 global interrupt.
 */
void DMA1_Stream3_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

    /* USER CODE END DMA1_Stream3_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_uart7_rx);
    /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

    /* USER CODE END DMA1_Stream3_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream5 global interrupt.
 */
void DMA1_Stream5_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

    /* USER CODE END DMA1_Stream5_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_usart2_rx);
    /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

    /* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
 * @brief This function handles DMA1 stream6 global interrupt.
 */
void DMA1_Stream6_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Stream6_IRQn 0 */

    /* USER CODE END DMA1_Stream6_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_uart8_rx);
    /* USER CODE BEGIN DMA1_Stream6_IRQn 1 */

    /* USER CODE END DMA1_Stream6_IRQn 1 */
}

/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */

    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&uartInterfaces[0].uartHandler);
    /* USER CODE BEGIN USART1_IRQn 1 */

    /* USER CODE END USART1_IRQn 1 */
}

/**
 * @brief This function handles USART2 global interrupt.
 */
void USART2_IRQHandler(void)
{
    /* USER CODE BEGIN USART2_IRQn 0 */

    /* USER CODE END USART2_IRQn 0 */
    HAL_UART_IRQHandler(&uartInterfaces[1].uartHandler);
    /* USER CODE BEGIN USART2_IRQn 1 */

    /* USER CODE END USART2_IRQn 1 */
}

/**
 * @brief This function handles USART3 global interrupt.
 */
void USART3_IRQHandler(void)
{
    /* USER CODE BEGIN USART3_IRQn 0 */

    /* USER CODE END USART3_IRQn 0 */
    HAL_UART_IRQHandler(&uartInterfaces[2].uartHandler);
    /* USER CODE BEGIN USART3_IRQn 1 */

    /* USER CODE END USART3_IRQn 1 */
}

/**
 * @brief This function handles UART4 global interrupt.
 */
void UART4_IRQHandler(void)
{
    /* USER CODE BEGIN UART4_IRQn 0 */

    /* USER CODE END UART4_IRQn 0 */
    HAL_UART_IRQHandler(&uartInterfaces[3].uartHandler);
    /* USER CODE BEGIN UART4_IRQn 1 */

    /* USER CODE END UART4_IRQn 1 */
}

/**
 * @brief This function handles UART5 global interrupt.
 */
void UART5_IRQHandler(void)
{
    /* USER CODE BEGIN UART5_IRQn 0 */

    /* USER CODE END UART5_IRQn 0 */
    HAL_UART_IRQHandler(&uartInterfaces[4].uartHandler);
    /* USER CODE BEGIN UART5_IRQn 1 */

    /* USER CODE END UART5_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void DMA2_Stream1_IRQHandler(void)
{
    /* USER CODE BEGIN DMA2_Stream1_IRQn 0 */

    /* USER CODE END DMA2_Stream1_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_usart6_rx);
    /* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

    /* USER CODE END DMA2_Stream1_IRQn 1 */
}

/**
 * @brief This function handles DMA2 stream2 global interrupt.
 */
void DMA2_Stream2_IRQHandler(void)
{
    /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

    /* USER CODE END DMA2_Stream2_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
    /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

    /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
 * @brief This function handles USART6 global interrupt.
 */
void USART6_IRQHandler(void)
{
    /* USER CODE BEGIN USART6_IRQn 0 */

    /* USER CODE END USART6_IRQn 0 */
    HAL_UART_IRQHandler(&uartInterfaces[5].uartHandler);
    /* USER CODE BEGIN USART6_IRQn 1 */

    /* USER CODE END USART6_IRQn 1 */
}

/**
 * @brief This function handles UART7 global interrupt.
 */
void UART7_IRQHandler(void)
{
    /* USER CODE BEGIN UART7_IRQn 0 */

    /* USER CODE END UART7_IRQn 0 */
    HAL_UART_IRQHandler(&uartInterfaces[6].uartHandler);
    /* USER CODE BEGIN UART7_IRQn 1 */

    /* USER CODE END UART7_IRQn 1 */
}

/**
 * @brief This function handles UART8 global interrupt.
 */
void UART8_IRQHandler(void)
{
    /* USER CODE BEGIN UART8_IRQn 0 */

    /* USER CODE END UART8_IRQn 0 */
    HAL_UART_IRQHandler(&uartInterfaces[7].uartHandler);
    /* USER CODE BEGIN UART8_IRQn 1 */

    /* USER CODE END UART8_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
