#include "I2C2.h"
#include "BoardSetup.h"

#define I2Cx I2C2


static i2cState_e i2cMode;
static i2cPacket_t i2cPacket;
static uint16_t i2cErrorCode;
static uint8_t*	pI2cBuff;

static uint8_t ErrorI2C_ISR_NACKF;
static uint8_t ErrorI2C_ICR_OVRCF; 
static uint8_t ErrorI2C_ICR_ARLOCF; 
static uint8_t ErrorI2C_ICR_BERRCF;

static systemticks_t I2Cx_starttime;
/*************************************************************************************************************************
*
*
**************************************************************************************************************************/

	/**
	 * @brief   do i2c transfer, read or write.
	 * @pre		no time control
	 *
	 * @param[in] p1
	 * @param[in] p2
	 * @param[in] p3
	 * @param[in] p4
	 * @param[in] p5
	 *
	 * @api
	 */


void I2C2_IRQHandler(void){
	
	uint16_t isrVal = I2Cx->ISR;
  
	if(isrVal & I2C_ISR_TXIS){
 		
		if(i2cPacket.numOfSubAddrBytes > 0){
			
			i2cPacket.numOfSubAddrBytes--;				
			I2Cx->TXDR = i2cPacket.subAddr[i2cPacket.numOfSubAddrBytes];			
     // if((i2cPacket.numOfSubAddrBytes == 0) && (i2cPacket.direct == TRANSFER_READ))
     //                                                                               { I2Cx->CR1 &= ~I2C_CR1_TXIE;}; 
				
		} else if ((i2cPacket.numOfDataBytes > 0) && (i2cPacket.direct == TRANSFER_WRITE)){	
			
       I2Cx->TXDR = *pI2cBuff++;
       i2cPacket.numOfDataBytes--;
      // if(i2cPacket.numOfDataBytes == 0){I2Cx->CR1 &= ~I2C_CR1_TXIE;}
			 
		 } 		
	}
	 
  if(isrVal & I2C_ISR_TC) 
		{ 
		
   // I2Cx->CR1 &= ~I2C_CR1_TCIE;                         	
   // i2cMode = (i2cPacket.direct == TRANSFER_READ) ? I2C_SUBADDR_TRANSFERED : I2C_TRANSACTION_OK;
   	 if (i2cPacket.direct == TRANSFER_WRITE) 	
		 {
			I2Cx->CR2 |= I2C_CR2_STOP;
		 }
     else
     {if 	(I2Cx->CR2 & I2C_CR2_RD_WRN)
        {			 
	        I2Cx->CR2 |= I2C_CR2_STOP;		
				}
  		else
			  { 				
          I2Cx->CR2 &= ~(I2C_CR2_NBYTES_Msk & I2C_CR2_RELOAD);  																							// clear NBYTES area 
          I2Cx->CR2 |= (1 << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN;	// set transfer bytes count and RD_WRN bit     
          if (i2cPacket.numOfDataBytes>1)I2Cx->CR2 |=I2C_CR2_RELOAD;				
          I2Cx->CR2 |= I2C_CR2_START;                                   									// repeated start 
		    } 
			}
     }
  
  if(isrVal & I2C_ISR_RXNE)
		{
		
     *pI2cBuff++ = I2Cx->RXDR;
     i2cPacket.numOfDataBytes--;
		};	
		
  if(isrVal & I2C_ISR_TCR){
	
    if(i2cPacket.numOfDataBytes == 1)
			{
			 I2Cx->CR2 &= ~(I2C_CR2_RELOAD_Msk);	
			 //I2Cx->CR2 &= ~I2C_CR2_NBYTES_Msk; 	
			 I2Cx->CR2 |= (1 << I2C_CR2_NBYTES_Pos);	
 //     I2Cx->CR1 &= ~I2C_CR1_RXIE;
 //     i2cMode = I2C_TRANSACTION_OK;
			
      }
			else
			{	
				I2Cx->CR2 &= ~I2C_CR2_NBYTES_Msk;  																							// clear NBYTES area 
        I2Cx->CR2 |= (1 << I2C_CR2_NBYTES_Pos);
			}
  }
   
	 
 if(isrVal & I2C_ISR_STOPF)
	 {
	  I2Cx->ICR = I2C_ICR_STOPCF;	
    i2cMode = I2C_IDLE;
	//I2Cx->CR1 &= ~(I2C_CR1_NACKIE | I2C_CR1_RXIE | I2C_CR1_TCIE | I2C_CR1_TXIE);    // disable interrupts
   } 	
	 
	 
 if(isrVal & I2C_ISR_NACKF)
	 {
	  I2Cx->ICR = I2C_ICR_NACKCF;	
    i2cMode = I2C_TRANSACTION_ERROR;
	//I2Cx->CR1 &= ~(I2C_CR1_NACKIE | I2C_CR1_RXIE | I2C_CR1_TCIE | I2C_CR1_TXIE);    // disable interrupts
		 ErrorI2C_ISR_NACKF++;
   } 	
	 
 if ( (isrVal & I2C_ICR_OVRCF) )
   {
		 I2Cx->ICR = I2C_ICR_OVRCF ;
	   i2cMode = I2C_TRANSACTION_ERROR;
		 i2cErrorCode=1;
		 ErrorI2C_ICR_OVRCF++;
	 };	 
	if (  (isrVal & I2C_ICR_ARLOCF)  )
   {
		 I2Cx->ICR =  I2C_ICR_ARLOCF ;
	   i2cMode = I2C_TRANSACTION_ERROR;
		 i2cErrorCode=1;
		 ErrorI2C_ICR_ARLOCF++;
	 };	 
if (  (isrVal & I2C_ICR_BERRCF) )
   {
		 I2Cx->ICR =  I2C_ICR_BERRCF;
	   i2cMode = I2C_TRANSACTION_ERROR;
		 i2cErrorCode=1;
		 ErrorI2C_ICR_BERRCF++;
	 };	 
	 
}

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
void initI2c2(void){
      
RCC->APBENR1 |= RCC_APBENR1_I2C2EN;											// I2C2 clock enable
  I2Cx->CR1 &= ~I2C_CR1_PE;																// I2C2 peripheral disable
	I2Cx->CR1 |= I2C_CR1_ANFOFF;														// Analog noise filter disabled
	
	I2Cx->TIMINGR |=  0xF0000000 |							            // TIMINGR register configuration for Standard-mode 100 kHz 
									 (0x13 << I2C_TIMINGR_SCLL_Pos) |
									 (0x0F << I2C_TIMINGR_SCLH_Pos) |
									 (0x02 << I2C_TIMINGR_SDADEL_Pos) |
									 (0x04 << I2C_TIMINGR_SCLDEL_Pos);    
	I2Cx->CR1	&= ~I2C_CR1_NOSTRETCH; 												// Clock stretching disable. It must be kept cleared in master mode
  I2Cx->CR2 &= ~(I2C_CR2_ADD10 |                          // master operates in 7-bit addressing mode
                 I2C_CR2_AUTOEND |                        // disable autoend mode 
                 I2C_CR2_RELOAD);                         // TCR flag is set when NBYTES data are transferred, stretching SCL low.
	I2Cx->CR1 |= I2C_CR1_PE;														    // I2C peripheral enable
	
  	
	NVIC_SetPriority(I2C2_IRQn,3);
  NVIC_EnableIRQ(I2C2_IRQn);
}


/***********************************************************************************************


************************************************************************************************/
void i2c2Start(void){
	
	i2cMode = I2C_BUSY; 
	i2cErrorCode = 0;
 	
  I2Cx->CR2 &= ~(I2C_CR2_NBYTES_Msk | I2C_CR2_SADD_Msk | I2C_CR2_RD_WRN_Msk |I2C_CR2_RELOAD_Msk);	// clear NBYTES, SADD data and RD_WRN bit 
	
	uint8_t transactionLen = i2cPacket.numOfSubAddrBytes;
	if (i2cPacket.direct == TRANSFER_WRITE) { transactionLen += i2cPacket.numOfDataBytes;}
	
  I2Cx->CR2 |= i2cPacket.slaveAddr | (transactionLen << I2C_CR2_NBYTES_Pos);	// set slave device address and transfer bytes count                          
  I2Cx->CR1 |= I2C_CR1_NACKIE | I2C_CR1_TCIE | I2C_CR1_TXIE | I2C_CR1_RXIE
            	| I2C_CR1_ERRIE | I2C_CR1_STOPIE ;	                         	// enable all interrupts	
  I2Cx->CR2 |= I2C_CR2_START;                                   							// start transaction
  I2Cx_starttime=SystemTicks;
}


/***********************************************************************************************


************************************************************************************************/
uint16_t i2c2DataRW(uint8_t SlaveAddress, transferMode_e tMode, uint16_t subAddr,  uint8_t subAddrLen, uint8_t* pDataBuff, uint8_t len )
{
	i2cPacket.direct = tMode;
  i2cPacket.slaveAddr = SlaveAddress << 1;
	i2cPacket.subAddr[0] = *(uint8_t*)&subAddr;
	i2cPacket.subAddr[1] = *((uint8_t*)&subAddr + 1);
	i2cPacket.numOfSubAddrBytes = subAddrLen;
	i2cPacket.numOfDataBytes = len;
	pI2cBuff = pDataBuff;  
  i2c2Start();
	
	return 255;
  
}


/***********************************************************************************************


************************************************************************************************/
#define MaxTransactionTime 3//ms
uint8_t getI2c2Status(void) 
{
	if (((SystemTicks-I2Cx_starttime)>MaxTransactionTime)&&(I2C_BUSY==i2cMode))
	{ i2cMode=I2C_TRANSACTION_ERROR;
		I2Cx->CR1 &= ~I2C_CR1_PE;};
	if ((I2C_TRANSACTION_ERROR==i2cMode)||(I2C_TRANSACTION_OK==i2cMode))
		i2cMode=I2C_IDLE;
		return i2cMode;
}
/***********************************************************************************************


************************************************************************************************/
uint8_t getI2c2Error(void) {
	return i2cErrorCode;
}

