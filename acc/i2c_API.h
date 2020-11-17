#ifndef __I2C_API_H__
#define __I2C_API_H__

//#include "i2c_soft.h"
#include "i2c_COMMON.h"

#include <stdint.h>

#define cPriorityDefault 10
#define cWriteTryCount 6



//typedef enum  {eI2CAPIIdle,eI2CAPIRequest,eI2CAPIActive,eI2CAPIDone,eI2CAPIDoneError} e_I2C_API_RequestState;
               //eIdle -> data has been read by high level function.
/*
typedef struct{
	int8_t  priority;

	t_I2cRecord I2cRecord;

	uint8_t *bufRW;
	
	e_FunctionReturnState state;
    void (*fun)(void);
} t_I2C_API_DATA;


extern t_I2C_API_DATA I2C_API_DATA[NumOfDevices];
*/
//extern int i2c_HANDLES[eI2CNumOfBusses];
//extern t_i2c_node* i2c_NODES[eI2CNumOfBusses];

extern void I2C_API_INIT(void);
extern void I2C_API_Reset(void);

extern  e_FunctionReturnState
I2C_API_Exchange( e_I2C_API_Devices d,	t_I2cRecord i2cRecord, uint8_t *buf, unsigned char priority,void (*fun)(uint8_t) );

extern e_FunctionReturnState
I2C_API_Wr_Check( e_I2C_API_Devices d,	t_I2cRecord i2cRecord, uint16_t data, unsigned char priority,void (*fun)(uint8_t));

//extern unsigned char state_of_I2Cpower_bus(e_I2C_API_Buses bus);

#endif
