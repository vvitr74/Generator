/**
 * DriverBQ28z610.c
 *
 *  Created on: Feb 21, 2019
 *      Author: 2382
 STM32 it is little endian only.
 */


#include "DriverBQ28z610.h"
#include "i2c_API.h"

const t_I2c16InitData BQ28z610InitData[e_BQ28z610_NumOfReg]=
{
		{{I2C_OP_READ,0x06,		1,2},0} // e_BQ28z610_Temperature
	 ,{{I2C_OP_READ,0x08,		1,2},0} //e_BQ28z610_Voltage
	 ,{{I2C_OP_READ,0x2c,		1,2},0}  //e_BQ28z610_RelativeStateOfCharge
	 ,{{I2C_OP_READ,0x3e,		1,2},0}  //e_BQ28z610_AltManufacturerAccess
};

static uint8_t BQ28z610_ExchangeData[34];

e_FunctionReturnState BQ28z610_AltManufacturerAccessCommand(uint16_t command)
{
//	static uint16_t commandl;
	e_FunctionReturnState wrstate;
	t_I2cRecord I2cRecord;
	I2cRecord=BQ28z610InitData[e_BQ28z610_AltManufacturerAccess].I2cRecord;
	I2cRecord.op_type=I2C_OP_WRITE;
	*((uint16_t*)BQ28z610_ExchangeData)=command;
		wrstate=I2C_API_Exchange(   bq28z610,
	   	   	   	   	   				I2cRecord,
					                BQ28z610_ExchangeData,
									cPriorityDefault,
									voidfun8
								);
		return  wrstate;	
};

e_FunctionReturnState BQ28z610_AltManufacturerAccessDFWrite(uint16_t address, uint8_t * data, uint8_t size)
{
//	static uint16_t commandl;
	e_FunctionReturnState wrstate;
	t_I2cRecord I2cRecord;
	
	if (size>32)
	{
		size=32;
		//strcopy(GlobalErrorString,"BQ28z610_AltManufacturerAccessDF, size>32");
	};
	
	I2cRecord=BQ28z610InitData[e_BQ28z610_AltManufacturerAccess].I2cRecord;
	I2cRecord.op_type=I2C_OP_WRITE;
	I2cRecord.bufRW_qntByte=size+2;
	
	for (uint8_t i=0;i<size;i++)
	{ 
		BQ28z610_ExchangeData[i+2]=data[i];
	};
	
	*((uint16_t*)BQ28z610_ExchangeData)=address;
	
		wrstate=I2C_API_Exchange(   bq28z610,
	   	   	   	   	   				I2cRecord,
					                BQ28z610_ExchangeData,
									cPriorityDefault,
									voidfun8
								);
		return  wrstate;	
};


e_FunctionReturnState BQ28z610_Read(e_BQ28z610_Registers reg, uint16_t *data)
{
	  static uint16_t dataRead;
	e_FunctionReturnState wrstate;
	t_I2cRecord I2cRecord;
	I2cRecord=BQ28z610InitData[reg].I2cRecord;
	I2cRecord.op_type=I2C_OP_READ;
		wrstate=I2C_API_Exchange(   bq28z610,
	   	   	   	   	   				I2cRecord,
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
