/*
 * I2C_COMMON.h
 *
 *  Created on: Feb 18, 2019
 *      Author: 2382
 */

#ifndef I2C_COMMON_H_
#define I2C_COMMON_H_

#include <stdint.h>

#define I2C_OP_READ         ((unsigned short)(0))
#define I2C_OP_WRITE        ((unsigned short)(1))

typedef struct {
					int8_t  op_type;

					uint16_t addrReg;
					uint8_t addrReg_qntByte;

					uint8_t  bufRW_qntByte;
               } t_I2cRecord;

typedef struct {

               	t_I2cRecord I2cRecord;
               	uint16_t data;

               } t_I2c16InitData;

typedef enum  {e_FRS_Done,e_FRS_Idle,e_FRS_Request,e_FRS_Processing,e_FRS_Busy,e_FRS_DoneError} e_FunctionReturnState;
                              //eIdle -> data has been read by high level function.

//typedef enum {eI2CofTPS82,eI2CofTPS86,eI2CofPB,eI2CNumOfBusses} e_I2C_API_Buses;
typedef enum  {TPS87,bq25703,bq28z610,NumOfDevices} e_I2C_API_Devices;


extern void voidfun(void);
extern void voidfun8(uint8_t a );


#endif /* I2C_COMMON_H_ */
