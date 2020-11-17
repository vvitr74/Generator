#include <stdint.h>

#include "superloop.h"

//#include "i2c_soft.h"
//#include "i2c_bus_pins.h"
#include "i2c_API.h"

#include "regBQ25703A.h"
#include "DriverBQ25703.h"
#include "DriverBQ28z610.h"
#include "DriverTPS65982_6.h"
#include "mainFSM.h"
#include "BoardSetup.h"
#include "I2c1.h"



uint8_t maintaskstate=15;


#define testkey (m_dcoff|m_sr82|m_p82|m_25703init|m_IinLow|m_hizOff|m_Iin82|m_IchAl|m_inhOff|m_DCon)

static uint16_t data_IIN_DPM;
static uint16_t data_ChargeCurrent;
static uint16_t data_ChargerStatus;
static uint16_t data_tmp;
static uint16_t pv_BQ28z610_Temperature;
static uint16_t I86;
static uint16_t V86;


//---------------------------------for power---------------------------------------------
//TPS65982_6_RW(e_I2C_API_Devices device, e_TPS65982_6_Registers reg, uint8_t *data, uint8_t qntByte, uint8_t RW);
//returnstateL1=TPS65982_6_RW(device,e_TPS65982_6_StatusRegister,data,255,I2C_OP_READ);
static unsigned char u8_11_ff[11]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};//ToDo do const
bool SuperLoop_Acc_SleepIn(void)
{ 
	e_FunctionReturnState returnstateL1;
	//bool returnstateL;
	returnstateL1=e_FRS_Processing;
	do 
		{
	    returnstateL1 = TPS65982_6_RW(TPS87,  e_TPS65987_IntClear1, u8_11_ff,  11,  I2C_OP_WRITE);
		}
  while ((e_FRS_Done!=returnstateL1)&&(e_FRS_DoneError!=	returnstateL1))	;

		return (e_FRS_Done==returnstateL1);
};

bool SuperLoop_Acc_SleepOut(void)
{
	return true;
};
//-------------------------------- FSM -------------------------------------------------


void maintask()
{e_FunctionReturnState returnstate;
e_FunctionReturnState rstatel;
  returnstate=e_FRS_Processing;

  switch(maintaskstate)
  {
   case 0:if (e_FRS_Done==BQ25703_Init_Check())
                 {maintaskstate++;};
	  	  break;
   case 1:if (e_FRS_Done==BQ25703_IIN_Check(100))
           {maintaskstate++;};
  	  	  break;
   case 2: if (e_FRS_Done==BQ25703_SetBits_Check(ChargeOption3,0,ChargeOption3_EN_HIZ))
                  {maintaskstate++;};
 	  	  break;
   case 3: if (e_FRS_Done==BQ25703_SetBits_Check(ChargeOption0,0,ChargeOption0_CHRG_INHIBIT))
		        {maintaskstate++;};
						break;
   case 4: if (e_FRS_Done==BQ25703_IIN_Check(400))
           {maintaskstate++;};
  	  	  break;
	 case 5: if (e_FRS_Done==BQ25703_Charge_Check(2500))
           {maintaskstate++;};
	       break;
	 case 6:  if (e_FRS_Done==BQ25703_Read(ChargeOption0,&data_tmp))
		        {maintaskstate++;};
  	  	  	  break;
	 case 7:  if (e_FRS_Done==BQ25703_Read(ChargerStatus,&data_ChargerStatus))
		        {maintaskstate++;};
  	  	  	  break;
	 case 8:  if (e_FRS_Done==BQ25703_Read(IIN_DPM,&data_IIN_DPM))
		        {maintaskstate++;};
  	  	  	  break;
   case 9: if (e_FRS_Done==BQ25703_SetBits_Check(ChargeOption0,ChargeOption0_CHRG_INHIBIT,0))
		        {maintaskstate++;};
						break;
   case 10:  if (e_FRS_Done==BQ25703_SetBits_Check(ChargeOption3,ChargeOption3_EN_HIZ,0))
                  {maintaskstate++;};         
           break;
				
   case 11: //if (e_FRS_Done==BQ28z610_Read(e_BQ28z610_Temperature,&pv_BQ28z610_Temperature))
		        {maintaskstate++;};
  	  	  	  break; 		 
   case 12: //if (e_FRS_Done==ControlBattery())
           {maintaskstate++;};
           break;
   case 13: if (e_FRS_Done==ReadTPSState())
           {maintaskstate++;};
           break;
   case 14: if (e_FRS_Done==TPS65982_6_RDO_R(TPS87,  &I86, &V86))
		         {maintaskstate++;};
//	       rstatel=MainTransition(testkey);
//	       if (e_FRS_Done==rstatel)
//                                 {maintaskstate++;};
//	       if (e_FRS_DoneError==rstatel)
//                                 {maintaskstate++;};
           break;
   case 15://do	{
							rstatel=mainFSMfunction(); 
					 SystemStatus=mainFMSstate;	 
		       if (e_FRS_Done==rstatel)
             {maintaskstate=15;};
					if (e_FRS_DoneError==rstatel)
             {maintaskstate=15;};//reset to init state	 
					// } while ((e_FRS_Done!=rstatel)&&(e_FRS_DoneError!=rstatel));
           break;

  default: maintaskstate=0;
  };

};



void SuperLoopACC_init(void)
{
//	type_sysTick_ms time;
//		time = get_sysTick_ms();
//i2c_init_node();
	initI2c1();
I2C_API_INIT();
//i2c_init_tasklocal();
BQ25703_DriverReset();
//while ((get_sysTick_ms()-time) <500) {;};
};

void SuperLoopACC(void)
{
//type_sysTick_ms time;
//	time = get_sysTick_ms();
//	i2c2_cycle_main(time, i2c_PB_HANDLE);
//	time = get_sysTick_ms();
//	if (1==state_of_I2Cpower_bus(eI2CofTPS82))
//	{i2c2_cycle_main(time, i2c_82_HANDLE);};
//	time = get_sysTick_ms();
//	//if (1==state_of_I2Cpower_bus(eI2CofTPS86))
//	{i2c2_cycle_main(time, i2c_86_HANDLE);};
	maintask();


};
