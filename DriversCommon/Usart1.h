
#ifndef		__USART1_H
#define   __USART1_H

#include "stm32g0xx.h"
#include "SuperLoop_Comm.h"

#define USART1_DIV (USART1_PCLK/USART1_BAUDRATE)
#define USART1_PCLK 64000000
#define USART1_BAUDRATE 115200
#define USB_RX_BUFF_SIZE 256

extern uint8_t usbRxBuff0[PAGE_SIZE];
extern uint8_t usbRxBuff1[PAGE_SIZE];

void uart1Init(void);

#endif
