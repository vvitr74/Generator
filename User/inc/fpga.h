#ifndef __FPGA_H
#define __FPGA_H

#include "stm32g0xx.h"

#define nCONFIG_H		GPIOB->BSRR = GPIO_BSRR_BS11
#define nCONFIG_L 	GPIOB->BSRR = GPIO_BSRR_BR11
#define FPGA_CS_H		GPIOB->BSRR = GPIO_BSRR_BS12
#define FPGA_CS_L		GPIOB->BSRR = GPIO_BSRR_BR12
#define FREQ_CW 			0x09
#define MULT_REG1_CW 	0x05
#define MULT_REG2_CW 	0x0A
#define CRC_CW				0xA0
#define FPGA_START_H GPIOA->BSRR = GPIO_BSRR_BS8
#define FPGA_START_L GPIOA->BSRR = GPIO_BSRR_BR8
#define CONF_BUFF_SIZE 1000

void fpgaConfig(void);
void confComplete(void);
void confFailed(void);
void fpgaControl(void);
void sendControlMessage(uint8_t contWord);
void setMult(uint8_t reg,uint16_t mult);
void getCrc(void);
void _setFreq(void);

//extern volatile fpgaFlags fpgaFlg;

#endif
