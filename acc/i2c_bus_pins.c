
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
#include "stm32l011xx.h"

#include "i2c_soft.h"
#include "i2c_bus_pins.h"
//


int i2c_PB_HANDLE = -1;
int i2c_82_HANDLE = -1;
int i2c_86_HANDLE = -1;

#define i2c_BIT_MASKA_1U		((uint32_t)1)



 //------------------------------------------------------------------------------
 //			I 2 C 		P B
 //------------------------------------------------------------------------------

#define i2c_PB_PORT_SCL_IN		((uint32_t *)&GPIOB->IDR)
#define i2c_PB_PORT_SCL_OUT		((uint32_t *)&GPIOB->BSRR)

#define i2c_PB_PORT_SDA_IN		((uint32_t *)&GPIOB->IDR)
#define i2c_PB_PORT_SDA_OUT		((uint32_t *)&GPIOB->BSRR)

#define i2c_PB_BSRR_BR_scl		((uint32_t)(i2c_BIT_MASKA_1U<<19))	//Pin 3 selected
#define i2c_PB_BSRR_BS_scl		((uint32_t)(i2c_BIT_MASKA_1U<<3))	//Pin 3 selected
#define i2c_PB_GPIO_IDR_scl		((uint32_t)(i2c_BIT_MASKA_1U<<3))	//Pin 3 selected

#define i2c_PB_BSRR_BR_sda		((uint32_t)(i2c_BIT_MASKA_1U<<20))	//Pin 4 selected
#define i2c_PB_BSRR_BS_sda		((uint32_t)(i2c_BIT_MASKA_1U<<4))	//Pin 4 selected
#define i2c_PB_GPIO_IDR_sda		((uint32_t)(i2c_BIT_MASKA_1U<<4))	//Pin 4 selected


//------------------------------------------------------------------------------
//			I 2 C 		8 2
//------------------------------------------------------------------------------
#define i2c_82_PORT_SCL_IN		((uint32_t *)&GPIOB->IDR)
#define i2c_82_PORT_SCL_OUT		((uint32_t *)&GPIOB->BSRR)

#define i2c_82_PORT_SDA_IN		((uint32_t *)&GPIOB->IDR)
#define i2c_82_PORT_SDA_OUT		((uint32_t *)&GPIOB->BSRR)

#define i2c_82_BSRR_BR_scl		((uint32_t)(i2c_BIT_MASKA_1U<<22))	//Pin 6 selected
#define i2c_82_BSRR_BS_scl		((uint32_t)(i2c_BIT_MASKA_1U<<6))	//Pin 6 selected
#define i2c_82_GPIO_IDR_scl		((uint32_t)(i2c_BIT_MASKA_1U<<6))	//Pin 6 selected

#define i2c_82_BSRR_BR_sda		((uint32_t)(i2c_BIT_MASKA_1U<<23))	//Pin 7 selected
#define i2c_82_BSRR_BS_sda		((uint32_t)(i2c_BIT_MASKA_1U<<7))	//Pin 7 selected
#define i2c_82_GPIO_IDR_sda		((uint32_t)(i2c_BIT_MASKA_1U<<7))	//Pin 7 selected


//------------------------------------------------------------------------------
//			I 2 C 		8 6
//------------------------------------------------------------------------------
#define i2c_86_PORT_SCL_IN		((uint32_t *)&GPIOA->IDR)
#define i2c_86_PORT_SCL_OUT		((uint32_t *)&GPIOA->BSRR)

#define i2c_86_PORT_SDA_IN		((uint32_t *)&GPIOA->IDR)
#define i2c_86_PORT_SDA_OUT		((uint32_t *)&GPIOA->BSRR)

#define i2c_86_BSRR_BR_scl		((uint32_t)(i2c_BIT_MASKA_1U<<25))	//Pin 9 selected
#define i2c_86_BSRR_BS_scl		((uint32_t)(i2c_BIT_MASKA_1U<<9))	//Pin 9 selected
#define i2c_86_GPIO_IDR_scl		((uint32_t)(i2c_BIT_MASKA_1U<<9))	//Pin 9 selected

#define i2c_86_BSRR_BR_sda		((uint32_t)(i2c_BIT_MASKA_1U<<26))	//Pin 10 selected
#define i2c_86_BSRR_BS_sda		((uint32_t)(i2c_BIT_MASKA_1U<<10))	//Pin 10 selected
#define i2c_86_GPIO_IDR_sda		((uint32_t)(i2c_BIT_MASKA_1U<<10))	//Pin 10 selected






 int i2c_init_node(void)
 {
 int i2c_HANDLE;

 	i2c_HANDLE = i2c_Open_New_node( i2c_PB_PORT_SCL_IN, i2c_PB_PORT_SCL_OUT, i2c_PB_PORT_SDA_IN, i2c_PB_PORT_SDA_OUT,
 								i2c_PB_BSRR_BR_scl, i2c_PB_BSRR_BS_scl, i2c_PB_GPIO_IDR_scl, i2c_PB_BSRR_BR_sda, i2c_PB_BSRR_BS_sda, i2c_PB_GPIO_IDR_sda);
 	if ( i2c_HANDLE == -1 )	return -1;
 	i2c_PB_HANDLE = i2c_HANDLE;

 	i2c_HANDLE = i2c_Open_New_node( i2c_82_PORT_SCL_IN, i2c_82_PORT_SCL_OUT, i2c_82_PORT_SDA_IN, i2c_82_PORT_SDA_OUT,
 								i2c_82_BSRR_BR_scl, i2c_82_BSRR_BS_scl, i2c_82_GPIO_IDR_scl, i2c_82_BSRR_BR_sda, i2c_82_BSRR_BS_sda, i2c_82_GPIO_IDR_sda);
 	if ( i2c_HANDLE == -1 )	return -2;
 	//if ( i2c_HANDLE != eI2CofTPS82 ) return -2;
 	i2c_82_HANDLE = i2c_HANDLE;

 	i2c_HANDLE = i2c_Open_New_node( i2c_86_PORT_SCL_IN, i2c_86_PORT_SCL_OUT, i2c_86_PORT_SDA_IN, i2c_86_PORT_SDA_OUT,
 								i2c_86_BSRR_BR_scl, i2c_86_BSRR_BS_scl, i2c_86_GPIO_IDR_scl, i2c_86_BSRR_BR_sda, i2c_86_BSRR_BS_sda, i2c_86_GPIO_IDR_sda);
 	if ( i2c_HANDLE == -1 )	return -3;
 	//if ( i2c_HANDLE != eI2CofTPS86 ) return -3;
 	i2c_86_HANDLE = i2c_HANDLE;

 	return 0;
 }

/*
 int i2c_init_node(void)
 {
 int i2c_HANDLE;

 	i2c_HANDLE = i2c_Open_New_node( i2c_PB_PORT_SCL_IN, i2c_PB_PORT_SCL_OUT, i2c_PB_PORT_SDA_IN, i2c_PB_PORT_SDA_OUT,
 								i2c_PB_BSRR_BR_scl, i2c_PB_BSRR_BS_scl, i2c_PB_GPIO_IDR_scl, i2c_PB_BSRR_BR_sda, i2c_PB_BSRR_BS_sda, i2c_PB_GPIO_IDR_sda);
 	if ( i2c_HANDLE == -1 )	return -1;
 	if ( i2c_HANDLE != eI2CofPB ) return -1;

 	i2c_HANDLE = i2c_Open_New_node( i2c_82_PORT_SCL_IN, i2c_82_PORT_SCL_OUT, i2c_82_PORT_SDA_IN, i2c_82_PORT_SDA_OUT,
 								i2c_82_BSRR_BR_scl, i2c_82_BSRR_BS_scl, i2c_82_GPIO_IDR_scl, i2c_82_BSRR_BR_sda, i2c_82_BSRR_BS_sda, i2c_82_GPIO_IDR_sda);
 	if ( i2c_HANDLE == -1 )	return -2;
 	if ( i2c_HANDLE != eI2CofTPS82 ) return -2;

 	i2c_HANDLE = i2c_Open_New_node( i2c_86_PORT_SCL_IN, i2c_86_PORT_SCL_OUT, i2c_86_PORT_SDA_IN, i2c_86_PORT_SDA_OUT,
 								i2c_86_BSRR_BR_scl, i2c_86_BSRR_BS_scl, i2c_86_GPIO_IDR_scl, i2c_86_BSRR_BR_sda, i2c_86_BSRR_BS_sda, i2c_86_GPIO_IDR_sda);
 	if ( i2c_HANDLE == -1 )	return -3;
 	if ( i2c_HANDLE != eI2CofTPS86 ) return -3;

 	return 0;
 }
*/

