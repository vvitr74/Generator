/*
 * DriverBQ28z610.h
 *
 *  Created on: Feb 21, 2019
 *      Author: 2382
 */

#ifndef DRIVERBQ28Z610_H_
#define DRIVERBQ28Z610_H_

#include "i2c_API.h"



typedef enum {
				e_BQ28z610_Temperature,
				e_BQ28z610_Voltage,
	      e_BQ28z610_RelativeStateOfCharge,
				e_BQ28z610_NumOfReg
} e_BQ28z610_Registers;




//extern e_FunctionReturnState
//BQ25703_Wr_Check( e_I2C_API_Devices d,	t_I2cRecord i2cRecord, uint16_t data, unsigned char priority,void (*fun)(uint8_t));
//extern e_FunctionReturnState BQ25703_Init();
//extern e_FunctionReturnState BQ25703_Work_Check();//work
//extern e_FunctionReturnState BQ25703_Sleep_Check();//sleep
extern e_FunctionReturnState BQ28z610_Read(e_BQ28z610_Registers reg, uint16_t *data);

extern void BQ28z610_DriverReset(void);


#endif /* DRIVERBQ28Z610_H_ */
