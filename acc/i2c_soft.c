
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
#include <string.h>
#include <stdbool.h>
//
//#include "gpio.h"
//#include "stm32l0xx_hal.h"

#include "i2c_soft.h"
//#include "user_task.h"
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#define I2C_ASK             ((unsigned char)(0))
#define I2C_NACK            ((unsigned char)(1))


#define I2C_BUS_TIME_OUT_AFTER_WR       ((type_sysTick_ms)(50))
#define I2C_BUS_TIME_OUT_SET_SCL		((type_sysTick_ms)(2))
#define I2C_BUS_NODE_MAX       			4


t_i2c_node i2c_node[eI2CNumOfBusses];

static int i2c_node_work = 0;


void i2c2_init(t_i2c_node *node);
void i2c2_start(t_i2c_node *node);
void i2c2_restart(t_i2c_node *node);
unsigned char i2c2_stop(t_i2c_node *node);
unsigned char i2c2_send_byte(t_i2c_node *node, unsigned char data);
unsigned char i2c2_read_byte(t_i2c_node *node, unsigned char ask);

static type_sysTick_ms  sysTick_ms = 0;

unsigned char get_SDA(t_i2c_node *node);
unsigned char get_SCL(t_i2c_node *node);


void inc_sysTick_ms(void)
{
	sysTick_ms++;
}
type_sysTick_ms get_sysTick_ms(void)
{
	return sysTick_ms;
}





unsigned char state_of_I2Cpower(t_i2c_node *node)
{

	e_i2c_bus_state bus_state =node->bus_state;
	if ((I2C_BUS_IDLE==bus_state)
	  ||(I2C_BUS_DONE_OK==bus_state)
	  ||(I2C_BUS_DONE_ERROR==bus_state)
	  ||(I2C_BUS_PAUSE_AFTER_STOP==bus_state)
	   )
		{node->I2Cpower_state=get_SCL(node)&get_SDA(node);};
	if (I2C_BUS_INIT==bus_state)
       	{node->I2Cpower_state=1;};
	return  node->I2Cpower_state;
}




e_i2c_bus_state i2c2_get_bus_state(t_i2c_node *node)
{
	return node->bus_state;
}

int i2c_Open_New_node( 	uint32_t *pSCL_in,    uint32_t *pSCL_out,   uint32_t *pSDA_in,  		uint32_t *pSDA_out,
						uint32_t BSRR_BR_scl, uint32_t BSRR_BS_scl, uint32_t GPIO_IDR_scl,
						uint32_t BSRR_BR_sda, uint32_t BSRR_BS_sda, uint32_t GPIO_IDR_sda )
{
int i2c_HANDLE;
t_i2c_node *node;	
	
		if ( i2c_node_work >= I2C_BUS_NODE_MAX )	return -1;
		node = (t_i2c_node *)&i2c_node[i2c_node_work];
		memset((void *)node, 0, sizeof(t_i2c_node));
		node->bus_state = I2C_BUS_INIT;
		node->pSCL_in   = pSCL_in;	
		node->pSCL_out  = pSCL_out;		
		node->pSDA_in   = pSDA_in;
		node->pSDA_out  = pSDA_out;
	
		node->GPIO_IDR_scl = GPIO_IDR_scl;
		node->GPIO_IDR_sda = GPIO_IDR_sda;

		node->BSRR_BR_scl = BSRR_BR_scl;	
		node->BSRR_BS_scl = BSRR_BS_scl;
		node->BSRR_BR_sda = BSRR_BR_sda;	
		node->BSRR_BS_sda = BSRR_BS_sda;

		i2c_HANDLE = i2c_node_work;
		i2c_node_work++;
		return i2c_HANDLE;
}

t_i2c_node *get_i2c_ptr_node(int i2c_HANDLE)
{
		return (t_i2c_node *)&i2c_node[i2c_HANDLE];
}	

int i2c2_bus_is_IDLE(t_i2c_node *node)
{
	
	
    if ( node->bus_state == I2C_BUS_IDLE )
        return 1;
    else
        return 0;        
}

int i2c2_bus_is_DONE(t_i2c_node *node)
{
    if ( (node->bus_state==I2C_BUS_DONE_ERROR) || (node->bus_state==I2C_BUS_DONE_OK) )
        return 1;
    else
        return 0;        
}

int i2c2_busHANDLE_is_DONE(int i2c2_busHANDLE)
{
t_i2c_node *node;

		node = get_i2c_ptr_node(i2c2_busHANDLE);
		return i2c2_bus_is_DONE(node);
}

int i2c2_bus_set_IDLE_if_DONE(t_i2c_node *node)
{
    if ( (node->bus_state==I2C_BUS_DONE_ERROR) || (node->bus_state==I2C_BUS_DONE_OK) )
    {
        node->bus_state = I2C_BUS_IDLE;
        return 0;
    }
    return -1;    
}

int i2c2_bus_get_rdData(t_i2c_node *node,   unsigned char *pBuf)
{
union dat_conv dc;

    if ( (node->bus_state==I2C_BUS_DONE_ERROR) || (node->bus_state==I2C_BUS_DONE_OK) )
    {

        dc.l4[0] = 0;
        dc.uc[0] = node->qnt_byte;
        return dc.l4[0];
    }
    return -1;    
}

int i2c2_bus_get_cntError(t_i2c_node *node)
{
    return node->cntError;
}


int i2c2_RW(t_i2c_node *node, unsigned char device_SlaveAddr, unsigned short addrReg,  unsigned char addrReg_qntByte,
		        unsigned char *pBuf, unsigned char qnt_byte, unsigned char op_type)
{
union dat_conv dc;    

    if ( i2c2_bus_is_IDLE(node) == 0 )  return -1;
    node->op_type = op_type;//I2C_OP_WRITE;I2C_OP_READ

    node->buf=pBuf;
    node->SlaveAddr = device_SlaveAddr;
    node->qnt_byte_tek = 0;
    dc.us[0] = addrReg;
    node->addrReg_H = dc.uc[1];
    node->addrReg_L = dc.uc[0];    
    node->addrReg_qntByte = addrReg_qntByte;
    node->qnt_byte=qnt_byte;
    node->cntError = 0;
    node->bus_state = I2C_BUS_START;
    return 0;
}



void i2c2_cycle_main(type_sysTick_ms time, int i2c_HANDLE)
{
unsigned char data, ret;
unsigned char ask;
t_i2c_node *node;
	
	node = get_i2c_ptr_node(i2c_HANDLE);
    switch ( node->bus_state )
    {
		case I2C_BUS_INIT:
            i2c2_init(node);
            node->bus_state = I2C_BUS_IDLE;
            break;
            
        case I2C_BUS_IDLE:
            break;
            
        case I2C_BUS_START:
        	node->I2Cpower_state=get_SCL(node)&get_SDA(node);
            i2c2_start(node);
            ret = i2c2_send_byte(node, node->SlaveAddr);
            if ( ret == I2C_NACK )
            {
                node->cntError++;
            }
			if ( node->qnt_byte == 0 )
				node->bus_state = I2C_BUS_STOP;
			else
				node->bus_state = I2C_BUS_ADDR_REG_H_beforeRST;
    		break;             
            
        case I2C_BUS_ADDR_REG_H_beforeRST:
            if ( node->addrReg_qntByte == 2 )
            {
                ret = i2c2_send_byte(node, node->addrReg_H);
                if ( ret == I2C_NACK )
                {
                    node->cntError++;
                }
            }
            node->bus_state = I2C_BUS_ADDR_REG_L_beforeRST;                        
            break;

        case I2C_BUS_ADDR_REG_L_beforeRST:
            ret = i2c2_send_byte(node, node->addrReg_L);
            if ( ret == I2C_NACK )
            {
                node->cntError++;
            }
            if ( node->op_type == I2C_OP_READ )
                node->bus_state = I2C_BUS_RESTART;
            else
                node->bus_state = I2C_BUS_WR_DATA;                
            break;
//------------------------------------------------------------------------------            
        case I2C_BUS_RESTART:
            i2c2_restart(node);
            ret = i2c2_send_byte(node, node->SlaveAddr | 0x01);
            if ( ret == I2C_NACK )
            {
                node->cntError++;
            }
            node->bus_state = I2C_BUS_RD_DATA;                        
            break;
            
        case I2C_BUS_RD_DATA:
            if ( node->qnt_byte_tek < (node->qnt_byte - 1) )
                ask = I2C_ASK;
            else
                ask = I2C_NACK;                
            data = i2c2_read_byte(node, ask);
            node->buf[node->qnt_byte_tek] = data;
            node->qnt_byte_tek++;
            if ( node->qnt_byte_tek >= node->qnt_byte )
            {
                node->bus_state = I2C_BUS_STOP;                                    
            }
            break;
//------------------------------------------------------------------------------                            
        case I2C_BUS_WR_DATA:
            data = node->buf[node->qnt_byte_tek];
            ret = i2c2_send_byte(node, data);
            if ( ret == I2C_NACK )
            {
                node->cntError++;
            }
            node->qnt_byte_tek++;
            if ( node->qnt_byte_tek >= node->qnt_byte )
            {
                node->bus_state = I2C_BUS_STOP;
            }
            break;            
            
        case I2C_BUS_STOP:
            i2c2_stop(node);
/*            if ( node->op_type == I2C_OP_READ )
            {
                if ( node->cntError == 0 )
                    node->bus_state = I2C_BUS_DONE_OK;
                else
                    node->bus_state = I2C_BUS_DONE_ERROR;
            }
            else
*/            {
                node->time = time;                           
                node->bus_state = I2C_BUS_PAUSE_AFTER_WR;                                            
            }
            break;                        

        case I2C_BUS_PAUSE_AFTER_WR:
            if ( (time - node->time) >= I2C_BUS_TIME_OUT_AFTER_WR )
            {
                if ( node->cntError == 0 )
                    node->bus_state = I2C_BUS_DONE_OK;
                else
                    node->bus_state = I2C_BUS_DONE_ERROR;
            }
            break;
/*        case I2C_BUS_PAUSE_AFTER_STOP:
            if ( (time - node->time) >= I2C_BUS_TIME_OUT_AFTER_WR )
            {
                if ( node->cntError == 0 )
                    node->bus_state = I2C_BUS_DONE_OK;
                else
                    node->bus_state = I2C_BUS_DONE_ERROR;
            }
            break;
*/        case I2C_BUS_DONE_OK: break;
        case I2C_BUS_DONE_ERROR: break;
        case I2C_BUS_PAUSE_AFTER_STOP: node->bus_state=I2C_BUS_IDLE; break;
            break;            
    }
}

unsigned char get_SCL(t_i2c_node *node)
{
uint32_t value; 	
	value = *node->pSCL_in;
	value &= node->GPIO_IDR_scl;
		if ( value )
			return 1;
		else
			return 0;	
}
unsigned char get_SDA(t_i2c_node *node)
{
uint32_t value; 	
	value = *node->pSDA_in;
	value &= node->GPIO_IDR_sda;
		if ( value )
			return 1;
		else
			return 0;	
}
void one_SCL(t_i2c_node *node)   //some thing new
{
type_sysTick_ms  sysTick_ms_start;
	*node->pSCL_out = node->BSRR_BS_scl;
	my_10us();
	sysTick_ms_start = get_sysTick_ms();
	while ( !get_SCL(node) )
	{
		if ( (get_sysTick_ms() - sysTick_ms_start) < I2C_BUS_TIME_OUT_SET_SCL )
		{
			my_10us();
		}
		else
		{
			node->cntError++;
			node->bus_state = I2C_BUS_DONE_ERROR;
			break;
		}
	}
}
/*
void one_SCL(t_i2c_node *node)
{
	*node->pSCL_out = node->BSRR_BS_scl;
	while ( !get_SCL(node) )
	{
		my_10us();
	}
}
*/
void null_SCL(t_i2c_node *node)
{
	*node->pSCL_out = node->BSRR_BR_scl;  	
}
void one_SDA(t_i2c_node *node)
{
	*node->pSDA_out = node->BSRR_BS_sda;  	
}

void null_SDA(t_i2c_node *node)
{
	*node->pSDA_out = node->BSRR_BR_sda;  	
}


unsigned char i2c2_stop(t_i2c_node *node)
{
unsigned char error = 0;
    null_SCL(node);
    my_10us();
    null_SDA(node);
    my_10us();    
    
    one_SCL(node);    
    my_10us();
    one_SDA(node);
    my_10us();
    
    my_10us();
    my_10us();
    my_10us();
    my_10us();    
    
    if ( get_SDA(node) == 0 )   error = 2;
    if ( get_SCL(node) == 0 )   error |= 1;
    return error;
}

void i2c2_start(t_i2c_node *node)
{
    my_10us();
    null_SDA(node);
    my_10us();    
    null_SCL(node);
    my_10us();
}

void i2c2_restart(t_i2c_node *node)
{
    one_SDA(node);
    my_10us();
    one_SCL(node);    
    my_10us();
    
    null_SDA(node);
    my_10us();    
    null_SCL(node);
    my_10us();
}

void i2c2_init(t_i2c_node *node)
{
    one_SDA(node);
    one_SCL(node);        
    i2c2_stop(node);    
}
unsigned char i2c2_send_byte(t_i2c_node *node, unsigned char data)
{
unsigned short i;
unsigned char ask = I2C_ASK;
    my_10us();
    my_10us();    
    for ( i = 0; i < 8; i++ )
    {
        if ( (data & 0x80) == 0 )
            null_SDA(node);
        else
            one_SDA(node);
        my_10us();
        one_SCL(node);    
        my_10us();



        null_SCL(node);
        my_10us();                
        data = data << 1;
    }
    one_SDA(node);
    my_10us();    
    one_SCL(node);
    my_10us();
    if ( get_SDA(node) == 1 )
       ask = I2C_NACK;
    else
       ask = I2C_ASK;
    null_SCL(node);
    my_10us();
    my_10us();    
    return ask;
}


unsigned char i2c2_read_byte(t_i2c_node *node, unsigned char ask)
{
unsigned short i;
unsigned char byte;
    byte = 0;
    one_SDA(node);
    my_10us();
    my_10us();    
    for ( i = 0; i < 8; i++ )
    {
        byte = byte << 1;
        one_SCL(node);
        my_10us();
        if ( get_SDA(node) == 1 )
            byte |= 0x01;
        null_SCL(node);
        my_10us();        
    }
    if ( ask == I2C_ASK )
        null_SDA(node);
    else
        one_SDA(node);        
    my_10us();        
    one_SCL(node);
    my_10us();
    null_SCL(node);    
    my_10us();
    one_SDA(node);
    my_10us();
    my_10us();    
    return byte;
}

void my_10us1(void);

void my_10us(void)
{
	my_10us1();
	my_10us1();
}

void my_10us1(void)
{
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");

	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");

	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");

	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");

	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");

	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");


	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");

	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");

	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");

}


