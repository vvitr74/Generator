#include "stm32g0xx.h"

#define USART1_ALT_FUNC 0x01
#define USART1_DIV (USART1_PCLK/USART1_BAUDRATE)
#define USART1_PCLK 64000000UL
#define USART1_BAUDRATE 9600UL

void usbInit(void);
void usbIoPinsInit(void);
void usbUartInit(void);
