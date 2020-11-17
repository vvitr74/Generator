#include "fpga.h"
#include "tim3.h"
#include "w25qxx.h"
#include "flash.h"
#include "Spi.h"

uint32_t playParamArr[7];
//uint32_t playFreqArr[200];
extern volatile uint32_t playClk;
uint8_t byteBuff;
uint32_t bytesCnt;

extern volatile struct fpgaFlags{
	uint8_t fpgaConfigCmd				:1;
	uint8_t fpgaConfigComplete	:1;
}fpgaFlags;

//void fpgaConfig(void)
//{
//	uint8_t bytesConfBuff[CONF_BUFF_SIZE];
//	uint32_t bytesCnt;
//	uint32_t bytesRemain=CONF_FILE_SIZE;
//	uint16_t bytesToRead;
//	
//	SPI2->CR1 &= ~SPI_CR1_SPE;
//	SPI2->CR1 |= SPI_CR1_LSBFIRST;
//	SPI2->CR1 |= SPI_CR1_SPE;
//	GPIOB->BSRR=GPIO_BSRR_BR0;							//FPGA 1.2 V on
//	nCONFIG_H;
//	while(!(GPIOC->IDR & GPIO_IDR_ID7)){}
//	delay_ms(1);
//	FPGA_CS_L;															//for logger
//	while(bytesRemain>0){
//		bytesCnt+=bytesToRead;
//		bytesRemain=CONF_FILE_SIZE-bytesCnt;
//		if(bytesRemain>=CONF_BUFF_SIZE){
//			bytesToRead=CONF_BUFF_SIZE;
//		}
//		else{
//			bytesToRead=bytesRemain;
//		}
//		W25qxx_ReadBytes(bytesConfBuff,FIRST_CONF_BYTE+10+bytesCnt,bytesToRead);
//		spi2Transmit(bytesConfBuff, bytesToRead);
//		if(GPIOC->IDR & GPIO_IDR_ID6){
//			spi2Transmit(0x00, 1);
//			confComplete();
//			fpgaFlags.fpgaConfigComplete=1;
//			fpgaFlags.fpgaConfigCmd=0;
//			FPGA_CS_H;
//			SPI2->CR1 &= ~SPI_CR1_SPE;
//			SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
//			SPI2->CR1 |= SPI_CR1_SPE;
//			return;
//		}
//	}
//	FPGA_CS_H;
//	confFailed();
//	fpgaFlags.fpgaConfigComplete=0;
//	fpgaFlags.fpgaConfigCmd=0;
//}

//As in VV2
void fpgaConfig(void)											//
{
	uint32_t bytesCnt=0;
	uint8_t byteBuff;
	GPIOB->BSRR=GPIO_BSRR_BR0;							//FPGA 1.2 V on
	nCONFIG_H;
	while(!(GPIOC->IDR & GPIO_IDR_ID7)){}
	delay_ms(1);
	spi1FifoClr();
	FPGA_CS_L;															//for logger
	for(bytesCnt=0;bytesCnt<CONF_FILE_SIZE;bytesCnt++){
		W25qxx_ReadByte(&byteBuff,FIRST_CONF_BYTE+bytesCnt);
		spi2Transmit(&byteBuff, 1);
		if(GPIOC->IDR & GPIO_IDR_ID6){
			spi2Transmit(0x00, 1);
			confComplete();
			fpgaFlags.fpgaConfigComplete=1;
			fpgaFlags.fpgaConfigCmd=0;
			FPGA_CS_H;
			SPI2->CR1 &= ~SPI_CR1_SPE;
			SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
			SPI2->CR1 |= SPI_CR1_SPE;
			return;
		}
	}
	FPGA_CS_H;
	confFailed();
	fpgaFlags.fpgaConfigComplete=0;
	fpgaFlags.fpgaConfigCmd=0;
}



void fpgaControl(void)
{
	if(fpgaFlags.fpgaConfigCmd==1){
//		delay_ms(2000);
		GPIOB->BSRR=GPIO_BSRR_BS0;	//FPGA 1.2 V off
		delay_ms(100);
		fpgaConfig();
		
//		load freq as in VV4
//		setMult(1,21000);
//		getCrc();
//		setMult(2,21000);
//		getCrc();
//		_setFreq();
//		getCrc();
//		FPGA_START_H;
//		delay_ms(1);
//		FPGA_START_L;
		
		
	}
	if(playClk>=playParamArr[3]){
		playClk=0;
	}
}

void setMult(uint8_t reg,uint16_t mult)
{
	uint8_t buff[3];
	switch(reg){
		case 1:
			buff[0]=MULT_REG1_CW;
			break;
		case 2:
			buff[0]=MULT_REG2_CW;
			break;
		default:
			break;
	}
	buff[1]=mult>>8;
	buff[2]=mult&0xFF;
	FPGA_CS_L;
	spi2Transmit(buff,3);
	FPGA_CS_H;
}

void getCrc()
{
	uint8_t buff[2];
	buff[0]=CRC_CW;
	buff[1]=0x00;
	FPGA_CS_L;
	spi2Transmit(buff,2);
//	crc=SPI2->DR;
	FPGA_CS_H;
}

void _setFreq()
{
	uint8_t buff[1001];
	uint16_t ind;
	buff[0]=FREQ_CW;
	for(int i=0;i<200;i++){
//		playFreqArr[i]=1000000;
	}
	for(int i=0;i<200;i++){
		ind=5*i+1;
		buff[ind]=0x00;
		buff[ind+1]=0x00;
//		buff[ind+2]=(playFreqArr[i]&0x00FFFFFF)>>16;
//		buff[ind+3]=(playFreqArr[i]&0x00FFFFFF)>>8;
//		buff[ind+4]=(playFreqArr[i]&0x00FFFFFF);
	}
	while(!(SPI2->SR & SPI_SR_TXE)){}
	FPGA_CS_L;
	spi2Transmit(buff,1001);
	FPGA_CS_H;
}

//void setFreq()
//{
////	uint8_t buff[1001];
//	uint16_t ind;
////	buff[0]=FREQ_CW;

//	for(int i=0;i<200;i++){
//		ind=5*i+1;
////		buff[ind]=0x00;
////		buff[ind+1]=0x00;
////		buff[ind+2]=(playFreqArr[i]&0x00FFFFFF)>>16;
////		buff[ind+3]=(playFreqArr[i]&0x00FFFFFF)>>8;
////		buff[ind+4]=(playFreqArr[i]&0x00FFFFFF);
//	}
//	while(!(SPI2->SR & SPI_SR_TXE)){}
//	FPGA_CS_L;
////	spi2Transmit(buff,1001);
//	FPGA_CS_H;
//}


void confComplete(void)
{
	uint8_t report[]="Configuration complete\r\n";
	for(int i=0;i<sizeof(report)-1;i++){
		USART1->TDR=report[i];
		while(!(USART1->ISR & USART_ISR_TC)){}
	}
}


void confFailed(void)
{
	uint8_t report[]="Configuration failed\r\n";
	for(int i=0;i<sizeof(report)-1;i++){
		USART1->TDR=report[i];
		while(!(USART1->ISR & USART_ISR_TC)){}
	}
}

