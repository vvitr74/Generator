#include "usb.h"

//uint16_t uart1IrqCnt=0;
//uint8_t usbRespArr[256];

void usbInit(void)
{
	usbIoPinsInit();
	usbUartInit();
}

void usbIoPinsInit(void)
{
	
}

void usbUartInit(void)
{
	RCC->APBENR2 |= RCC_APBENR2_USART1EN;
	USART1->BRR = USART1_DIV;		//sets UART1 baudrate 9600 baud
	USART1->CR3 |= USART_CR3_ONEBIT;
	USART1->CR1 |= //USART_CR1_FIFOEN |
									USART_CR1_RXNEIE_RXFNEIE |
									USART_CR1_TE |
									USART_CR1_RE |
									USART_CR1_UE;
	NVIC_SetPriority(USART1_IRQn,1);
	NVIC_EnableIRQ(USART1_IRQn);
}

//void USART1_IRQHandler(void)
//{
//	if(USART1->ISR & USART_ISR_RXNE_RXFNE){
//		USART1->ICR |= USART_ICR_ORECF;	
//		usbRespArr[uart1IrqCnt]=USART1->RDR;
//		uart1IrqCnt++;
//	}
//}
