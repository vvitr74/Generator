#include "Spi2.h"

void initSpi_2(void){
  RCC->APBENR1 |= RCC_APBENR1_SPI2EN;                     // enable SPI2 clk
  SPI2->CR1 &= ~SPI_CR1_SPE;                              // disable SPI2 perif
  SPI2->CR1 |= /*SPI_CR1_BR_2 |*/ SPI_CR1_BR_0 | //SPI_CR1_BR_1 |  
               SPI_CR1_SSM |															// SPI_NSS_Soft
               SPI_CR1_SSI |
							 SPI_CR1_LSBFIRST |
//               SPI_CR1_CPOL |															// SPI_CPOL_High
//               SPI_CR1_CPHA |															// SPI_CPHA_2Edge
               SPI_CR1_MSTR;                              // master mode select
	SPI2->CR2 |= SPI_CR2_FRXTH;															// RXNE event is generated if the FIFO level is greater than or equal to 1/4 (8-bit)
//							 SPI_CR2_RXNEIE;														// enable SPI2 RXNE interrupt
  SPI2->CR1 |= SPI_CR1_SPE;                               // enable SPI2 perif
}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
void spi2Transmit(uint8_t *pData, uint16_t Size)
{
	uint16_t TxXferCount=Size;
//	uint16_t Temp;
	  /* Transmit data in 8 Bit mode */
	while (TxXferCount > 0U){
		/* Wait until TXE flag is set to send data */
		if (SPI2->SR & SPI_SR_TXE){
			if (TxXferCount > 1U){
				/* write on the data register in packing mode */
				SPI2->DR = *((uint16_t *)pData);
				pData += sizeof(uint16_t);
				TxXferCount -= 2U;
			}
			else{
				*(__IO uint8_t *)&SPI2->DR = *pData;
				pData++;
				TxXferCount--;
			}
		}
	}
	while(SPI2->SR & SPI_SR_BSY){}
}

