#include "global.h"

UartInterface uartInterfaces[8];
NamedUARTInterface namedUARTInterface;
TestBoardStatus testBoardStatus[2];
uint32_t lastMainBoardResponseTick = 0;
bool isMainBoardConnected = false;

void restore15VChannels() {
    debugMessage("Restore 15V channel....\n");
    HAL_GPIO_WritePin(GPIOD, TB0_15V_Pin|TB1_15V_Pin, GPIO_PIN_SET);
}

UartInterface * getUARTInterface(UART_HandleTypeDef *huart) {
    for (int i = 0; i < 8; i++) {
        if (huart == &(uartInterfaces[i].uartHandler)) {
            return &uartInterfaces[i];
        }
    }
    return NULL;
}
