
#ifndef __I2C_SOFT_H__
#define __I2C_SOFT_H__

//#include <stdint.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "I2C_COMMON.h"



#define I2C_OP_READ         ((unsigned short)(0))
#define I2C_OP_WRITE        ((unsigned short)(1))


//#define buff_size_RxTx      64

typedef unsigned long type_sysTick_ms;


union dat_conv
{
    char sc[8];
    unsigned char uc[8];
    unsigned short us[4];
    signed short ss[4];
    unsigned long ul[2];
    long l4[2];
    float f[2];
    double d;
    long long ll;
};

typedef enum {
    I2C_BUS_INIT = 0,
    I2C_BUS_IDLE = 1,
    I2C_BUS_START = 2,
    I2C_BUS_ADDR_REG_H_beforeRST = 3,
    I2C_BUS_ADDR_REG_L_beforeRST = 4,
    I2C_BUS_WR_DATA = 5,
    I2C_BUS_RESTART = 6,                        
    I2C_BUS_RD_DATA = 7,
    I2C_BUS_STOP = 8, 
    I2C_BUS_DONE_OK = 9,            
    I2C_BUS_DONE_ERROR = 10,
    I2C_BUS_PAUSE_AFTER_WR = 11,
	I2C_BUS_PAUSE_AFTER_STOP=12
} e_i2c_bus_state;



typedef struct {
	uint32_t *pSCL_in;
	uint32_t *pSDA_in;
	uint32_t *pSCL_out;	
	uint32_t *pSDA_out;
	
	uint32_t BSRR_BR_scl;
	uint32_t BSRR_BS_scl;
	uint32_t BSRR_BR_sda;
	uint32_t BSRR_BS_sda;
	
	uint32_t GPIO_IDR_scl;
	uint32_t GPIO_IDR_sda;
	
	e_i2c_bus_state bus_state;
	unsigned char I2Cpower_state;
	unsigned char SlaveAddr;
	unsigned char addrReg_H;
	unsigned char addrReg_L;
	unsigned char addrReg_qntByte;
	unsigned char qnt_byte;
	unsigned char qnt_byte_tek;
	unsigned char *buf;
	unsigned char op_type;
	int cntError;
type_sysTick_ms time;	
} t_i2c_node;



extern t_i2c_node i2c_node[eI2CNumOfBusses];

int i2c2_bus_get_rdData(t_i2c_node *node,   unsigned char *pBuf);


void one_SCL(t_i2c_node *node);
void null_SCL(t_i2c_node *node);
int i2c2_bus_set_IDLE_if_DONE(t_i2c_node *node);
int i2c2_bus_is_IDLE(t_i2c_node *node);
e_i2c_bus_state i2c2_get_bus_state(t_i2c_node *node);

void my_10us(void);

void inc_sysTick_ms(void);
type_sysTick_ms get_sysTick_ms(void);

void i2c2_cycle_main(type_sysTick_ms time, int i2c_HANDLE);
t_i2c_node *get_i2c_ptr_node(int i2c_HANDLE);

int i2c2_busHANDLE_is_DONE(int i2c2_busHANDLE);
int i2c_Open_New_node( 	uint32_t *pSCL_in,    uint32_t *pSCL_out,   uint32_t *pSDA_in,  		uint32_t *pSDA_out,
						uint32_t BSRR_BR_scl, uint32_t BSRR_BS_scl, uint32_t GPIO_IDR_scl,
						uint32_t BSRR_BR_sda, uint32_t BSRR_BS_sda, uint32_t GPIO_IDR_sda );






int i2c2_RW(t_i2c_node *node, unsigned char device_SlaveAddr, unsigned short addrReg,  unsigned char addrReg_qntByte,
		        unsigned char *pBuf, unsigned char qnt_byte, unsigned char op_type);

extern unsigned char state_of_I2Cpower(t_i2c_node *node);

#endif /* __I2C_SOFT_H__ */


