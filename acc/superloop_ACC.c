/**
\file high level file for Control TPS65987, bq25703a, bq 28z610

For sleep mode:
used typedef enum  {e_PS_Work,e_PS_DontMindSleep,e_PS_ReadySleep} e_PowerState;
e_PowerState SLPl_GetPoewerState(void);
e_PowerState SLPl_SetSleepState(bool state);

using:
If all relevant modules have state e_PS_DontMindSleep or e_PS_ReadySleep -> 
send to all modules requires for sleep SLPl_SetSleepState(true)->
if all modules e_PS_ReadySleep-> sleep
if any modules e_PS_Work-> for all modules SLPl_SetSleepState(false)

*/

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
static e_PowerState SLAcc_PowerState; 
static bool SLAcc_GoToSleep;
__inline e_PowerState SLPo_GetPowerState(void)
{
	 return SLAcc_PowerState;
};

__inline e_PowerState SLAcc_SetSleepState(bool state)
{
	SLPo_GoToSleep=state;
	return SLPo_PowerState;
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
