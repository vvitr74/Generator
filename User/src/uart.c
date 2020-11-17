#include "uart.h"
#include "flash.h"

extern uint8_t usbRxBuff0[PAGE_SIZE];
extern uint8_t usbRxBuff1[PAGE_SIZE];
uint8_t usbBuffBytesCnt;
extern uint16_t filePagesCnt;
extern uint8_t lastPageBytesNum;
extern uint16_t filePagesNum;
extern uint32_t fileBytesNum;
extern uint32_t rxIrqCnt;
uint8_t usbCmdBuff[3];
extern uint8_t usbCmd;
extern uint8_t usbCmd;

volatile struct{
	uint16_t buffSel					:1;
	uint16_t getStartAddr			:1;
	uint16_t buff0DataRdy			:1;
	uint16_t buff1DataRdy			:1;
//	uint16_t firstPage				:1;
	uint16_t lastPage					:1;
//	uint16_t startWrite				:1;
	uint16_t stopWrite				:1;
	uint16_t uartDataRdy			:1;
}usbFlags;

void uart1Init(void)
{
	RCC->APBENR2 |= RCC_APBENR2_USART1EN;
	USART1->CR1 &= ~USART_CR1_UE;
	USART1->BRR = USART1_DIV;		//sets UART1 baudrate 115200 baud
	USART1->CR3 |= USART_CR3_ONEBIT;
	USART1->CR1 |= //USART_CR1_FIFOEN |
									USART_CR1_RXNEIE_RXFNEIE |
									USART_CR1_TE |
									USART_CR1_RE;
	NVIC_SetPriority(USART1_IRQn,1);
	NVIC_EnableIRQ(USART1_IRQn);
	USART1->CR1 |= USART_CR1_UE;
	
	usbFlags.buffSel=0;
	usbBuffBytesCnt=0;
}

void USART1_IRQHandler(void)
{
	if(USART1->ISR & USART_ISR_RXNE_RXFNE){
		USART1->ICR |= USART_ICR_ORECF;
		USART1->RQR |= USART_RQR_RXFRQ;
		if(rxIrqCnt<3){
			usbCmdBuff[rxIrqCnt]=USART1->RDR;
			if(rxIrqCnt==2){
				usbCmd=usbCmdBuff[0];
			}
			rxIrqCnt++;
			return;
		}
		USART1->ICR |= USART_ICR_IDLECF;
		USART1->CR1 |= USART_CR1_IDLEIE;
		rxIrqCnt++;
//		usbFlags.uartDataRdy=1;
		if(usbFlags.buffSel==0){
			usbRxBuff0[usbBuffBytesCnt]=USART1->RDR;
			if((usbBuffBytesCnt==PAGE_SIZE-1)/*||((usbBuffBytesCnt==PAGE_SIZE-11)&&(usbFlags.firstPage==1))*/){
				filePagesCnt++;
				usbBuffBytesCnt=0;
				usbFlags.buff0DataRdy=1;
				usbFlags.buffSel=1;
			}
			else{
				usbBuffBytesCnt++;
			}
		}
		else{
			usbRxBuff1[usbBuffBytesCnt]=USART1->RDR;
			if((usbBuffBytesCnt==PAGE_SIZE-1)/*||((usbBuffBytesCnt==PAGE_SIZE-11)&&(usbFlags.firstPage==1))*/){
				filePagesCnt++;
				usbBuffBytesCnt=0;
				usbFlags.buff1DataRdy=1;
				usbFlags.buffSel=0;
			}
			else{
				usbBuffBytesCnt++;
			}
		}
	}
	if(USART1->ISR & USART_ISR_IDLE){
		USART1->ICR |= USART_ICR_IDLECF;
		USART1->CR1 &= ~USART_CR1_IDLEIE;
		filePagesCnt++;
		usbFlags.lastPage=1;
		if(usbFlags.buffSel==0){
			usbFlags.buff0DataRdy=1;
		}
		else{
			usbFlags.buff1DataRdy=1;
		}
		lastPageBytesNum=usbBuffBytesCnt;
		filePagesNum=filePagesCnt;
		fileBytesNum=(filePagesNum-1)*PAGE_SIZE+lastPageBytesNum;
		usbBuffBytesCnt=0;
		filePagesCnt=0;
		usbFlags.stopWrite=1;
		rxIrqCnt=0;
	}
}

//void usbCmdDetect(void)
//{
//	if(rxIrqCnt==1){
//		usbCmd=USART1->RDR;
//	}
//}

//void rdPage(void)
//{
//	if(usbFlags.stopWrite==1){
//		filePagesCnt++;
//		usbFlags.lastPage=1;
//		if(usbFlags.buffSel==0){
//			usbFlags.buff0DataRdy=1;
//		}
//		else{
//			usbFlags.buff1DataRdy=1;
//		}
//		lastPageBytesNum=usbBuffBytesCnt;
//		filePagesNum=filePagesCnt;
//		fileBytesNum=(filePagesNum-1)*PAGE_SIZE+lastPageBytesNum;
//		usbBuffBytesCnt=0;
//		filePagesCnt=0;
//		usbFlags.stopWrite=0;
//	}
//	
//	if(usbFlags.uartDataRdy==1 && rxIrqCnt>2){
//		if(usbFlags.buffSel==0){
//			usbRxBuff0[usbBuffBytesCnt]=USART1->RDR;
//			if(usbBuffBytesCnt==PAGE_SIZE-1){
//				filePagesCnt++;
//				usbBuffBytesCnt=0;
//				usbFlags.buff0DataRdy=1;
//				usbFlags.buffSel=1;
//			}
//			else{
//				usbBuffBytesCnt++;
//			}
//		}
//		else{
//			usbRxBuff1[usbBuffBytesCnt]=USART1->RDR;
//			if(usbBuffBytesCnt==PAGE_SIZE-1){
//				filePagesCnt++;
//				usbBuffBytesCnt=0;
//				usbFlags.buff1DataRdy=1;
//				usbFlags.buffSel=0;
//			}
//			else{
//				usbBuffBytesCnt++;
//			}
//		}
//		usbFlags.uartDataRdy=0;
//	}
//}

//void USART2_IRQHandler(void)
//{
//	if(USART2->ISR & USART_ISR_RXNE_RXFNE){
//		
//	}
//}
