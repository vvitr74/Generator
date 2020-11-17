/*
 * DriverBQ28z610.c
 *
 *  Created on: Feb 21, 2019
 *      Author: 2382
 */


#include "DriverBQ28z610.h"
#include "i2c_API.h"

const t_I2c16InitData BQ28z610InitData[e_BQ28z610_NumOfReg]=
{
		{{I2C_OP_READ,0x06,		1,2},0}, // e_BQ28z610_Temperature
		{{I2C_OP_READ,0x08,		1,2},0}, //e_BQ28z610_Voltage
		{{I2C_OP_READ,0x2c,		1,2},0}  //e_BQ28z610_RelativeStateOfCharge
};

e_FunctionReturnState BQ28z610_Read(e_BQ28z610_Registers reg, uint16_t *data)
{
	  static uint16_t dataRead;
	e_FunctionReturnState wrstate;
	t_I2cRecord I2cRecordRead;
	I2cRecordRead=BQ28z610InitData[reg].I2cRecord;
	I2cRecordRead.op_type=I2C_OP_READ;
		wrstate=I2C_API_Exchange(   bq28z610,
	   	   	   	   	   				I2cRecordRead,
					                (uint8_t*)&dataRead,
									cPriorityDefault,
									voidfun8
								);

		if (e_FRS_Done==wrstate)
		                         {*data=dataRead;};
		if (e_FRS_DoneError==wrstate)
		                         {*data=0xfffe;};		
 //   if ((dataRead>4000)&&(reg==0))
	//	                         {*data=0xffff;};														 
		return  wrstate;
}

void BQ28z610_DriverReset(void)
{
	;
}
