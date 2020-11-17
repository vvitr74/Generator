#include "BoardSetup.h"
#include "Spi.h"
#include "tim3.h"
#include "w25qxx.h"

spiMode_en spiMode;
uint32_t spiRxNeedLen;

uint8_t TxBuff[16];
uint8_t *pTxBuff;
uint8_t *pSpiTxBuff;
uint16_t *pSpiWord;


uint32_t txallowed = 1U;

volatile struct {
  uint16_t BUSY 				    : 1;
  uint16_t MEMORY_INC_EN		: 1;
  uint16_t WORD_MODE_EN		  : 1;
}spiStatus;

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
void initSpi_1(void){
	NVIC_DisableIRQ(SPI1_IRQn); 
  RCC->APBENR2 |= RCC_APBENR2_SPI1EN;                     // enable SPI1 clk
  SPI1->CR1 &= ~SPI_CR1_SPE;                              // disable SPI1 perif
	SPI1->CR1 &= ~(
						 SPI_CR1_CPOL |															// SPI_CPOL_High
						 SPI_CR1_CPHA 													// SPI_CPHA_2Edge
						) ;                            
  SPI1->CR1 |= /*SPI_CR1_BR_2 |*/ SPI_CR1_BR_0 | //SPI_CR1_BR_1 |       
               SPI_CR1_SSM |															// SPI_NSS_Soft
               SPI_CR1_SSI |
//               SPI_CR1_CPOL |															// SPI_CPOL_High
//               SPI_CR1_CPHA |															// SPI_CPHA_2Edge
               SPI_CR1_MSTR;                              // master mode select
	SPI1->CR2 |= SPI_CR2_FRXTH;															// RXNE event is generated if the FIFO level is greater than or equal to 1/4 (8-bit)
  SPI1->CR1 |= SPI_CR1_SPE;                               // enable SPI1 perif
}


/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
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
void spiByteModeEnable(void){
  SPI1->CR1 &= ~SPI_CR1_SPE;                              // disable SPI1 perif
  SPI1->CR2 &= ~SPI_CR2_DS_Msk;
  SPI1->CR2 |= (0x7 << SPI_CR2_DS_Pos);
  SPI1->CR2 |= SPI_CR2_FRXTH;														  // RXNE event is generated if the FIFO level is greater than or equal to 1/4 (8-bit)
  spiStatus.WORD_MODE_EN = 0;
  SPI1->CR1 |= SPI_CR1_SPE;                               // enable SPI1 perif

}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
void spiWordModeEnable(void){
  SPI1->CR1 &= ~SPI_CR1_SPE;                              // disable SPI1 perif
  SPI1->CR2 &= ~SPI_CR2_DS_Msk;
  SPI1->CR2 |= (0xF << SPI_CR2_DS_Pos);
  SPI1->CR2 &= ~SPI_CR2_FRXTH;	
  spiStatus.WORD_MODE_EN = 1;  
  SPI1->CR1 |= SPI_CR1_SPE;                               // enable SPI1 perif
}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
uint16_t displayWriteCmdByte(uint8_t cmd){
	uint32_t timeout = 64000;
  spiMode = SPI_DISPLAY;
	spiStatus.BUSY = 1;
  spiByteModeEnable();
  pSpiTxBuff = (uint8_t*)&cmd;
  pTxBuff = (uint8_t*)TxBuff;
  spiRxNeedLen = 1;
  TFT_DC_CMD;
  TFT_CS_LOW;
	SPI1->DR = *pSpiTxBuff;
  while(spiStatus.BUSY && timeout){ timeout--;}
  TFT_CS_HI;    
  TFT_DC_DATA;
	return timeout;
}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
uint16_t displayWriteDataBytes(uint8_t* pDataByte, uint32_t len){
	uint32_t timeout = 64000;
  spiMode = SPI_DISPLAY;
	spiStatus.BUSY = 1;
  pSpiTxBuff = pDataByte;
  spiRxNeedLen = len;
  TFT_CS_LOW;
	SPI1->DR = *pSpiTxBuff;	
  while(spiStatus.BUSY && timeout){ timeout--;}
  TFT_CS_HI;    
	return timeout;
}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
uint16_t displayWriteDataWords(uint16_t* pDataWord, uint32_t len){
	uint32_t timeout = 64000;
  spiMode = SPI_DISPLAY;
	spiStatus.BUSY = 1;
  pSpiWord = pDataWord;
  spiRxNeedLen = len;
  TFT_CS_LOW;
	SPI1->DR = *pSpiWord;	
  while(spiStatus.BUSY && timeout){ timeout--;}
  TFT_CS_HI;    
	return timeout;
}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
uint16_t displayWriteOneWord(uint16_t DataWord){
	uint32_t timeout = 64000;
	spiStatus.BUSY = 1;
  spiRxNeedLen = 1;
	SPI1->DR = DataWord;	
  while(spiStatus.BUSY && timeout){ timeout--;} 
	return timeout;
}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
void spi1Receive(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint16_t RxXferCount=Size;
	uint16_t Temp;
	for(int i=0;i<2;i++){
		Temp=SPI1->DR;
	}
	*(__IO uint8_t *)&SPI1->DR = W25QXX_DUMMY_BYTE;
	while (RxXferCount > 0U){
//		*(__IO uint8_t *)&SPI1->DR = W25QXX_DUMMY_BYTE;
		if (SPI1->SR & SPI_SR_RXNE){
			/* read the received data */
			(* (uint8_t *)pData) = SPI1->DR;
			pData += sizeof(uint8_t);
			RxXferCount--;
			if(RxXferCount>0){
				*(__IO uint8_t *)&SPI1->DR = W25QXX_DUMMY_BYTE;
			}
		}
	}
	while(SPI1->SR & SPI_SR_BSY){}
}
/*************************************************************************************************************************
*
*
**************************************************************************************************************************/

void spi1Transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint16_t TxXferCount=Size;
	uint16_t Temp;
	  /* Transmit data in 8 Bit mode */
	while (TxXferCount > 0U){
		/* Wait until TXE flag is set to send data */
		if (SPI1->SR & SPI_SR_TXE){
			if (TxXferCount > 1U){
				/* write on the data register in packing mode */
				SPI1->DR = *((uint16_t *)pData);
				pData += sizeof(uint16_t);
				TxXferCount -= 2U;
			}
			else{
				*(__IO uint8_t *)&SPI1->DR = *pData;
				pData++;
				TxXferCount--;
			}
		}
	}
	while(SPI1->SR & SPI_SR_BSY){}
	
	for(int i=0;i<2;i++){
		Temp=SPI1->DR;
	}
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
		
//	for(int i=0;i<2;i++){
//		Temp=SPI2->DR;
//	}
}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/

void spi1TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
	uint16_t TxXferCount=Size;
	uint16_t RxXferCount=Size;
	uint16_t Temp;
	
	while ((TxXferCount > 0U) || (RxXferCount > 0U)){
      /* Check TXE flag */
		if ((SPI1->SR & SPI_SR_TXE) && (TxXferCount > 0U) && (txallowed == 1U)){
			if (TxXferCount > 1U){
				SPI1->DR = *((uint16_t *)pTxData);
				pTxData += sizeof(uint16_t);
				TxXferCount -= 2U;
			}
			else{
				*(__IO uint8_t *)&SPI1->DR = *pTxData;
				pTxData++;
				TxXferCount--;
			}
			/* Next Data is a reception (Rx). Tx not allowed */
			txallowed = 0U;
		}
			/* Wait until RXNE flag is reset */
		if ((SPI1->SR & SPI_SR_RXNE) && (RxXferCount > 0U)){
			if (RxXferCount > 1U){
				*((uint16_t *)pRxData) = SPI1->DR;
				pRxData += sizeof(uint16_t);
				RxXferCount -= 2U;
				if (RxXferCount <= 1U){
					/* Set RX Fifo threshold before to switch on 8 bit data size */
					spiByteModeEnable();
				}
			}
		else{
			*((uint16_t *)pRxData) = SPI1->DR;
			pRxData++;
			RxXferCount--;
		}
		/* Next Data is a Transmission (Tx). Tx is allowed */
			txallowed = 1U;
		}
	}
}
/*************************************************************************************************************************
*
*
**************************************************************************************************************************/

void spi1FifoClr(void)
{
	uint16_t Temp;
	
	for(int i=0;i<3;i++){
		Temp=SPI1->DR;
	}
}
/*************************************************************************************************************************
*
*
**************************************************************************************************************************/


/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
