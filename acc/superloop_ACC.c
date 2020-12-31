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

\todo clear interrupt pending
\todo exit mainFSMfunction on only on steady state - > need check

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

__inline e_PowerState SLAcc_GetPowerState(void)
{
	 return SLAcc_PowerState;
};

__inline e_PowerState SLAcc_SetSleepState(bool state)
{
	SLAcc_GoToSleep=state;
	return SLAcc_PowerState;
};
//-------------------------------- loop -------------------------------------------------

const e_PowerState encoderPowerState[16]=
{e_PS_DontMindSleep //0
,e_PS_ReadySleep   //1
,e_PS_Work //2
,e_PS_Work //3
,e_PS_Work //4
,e_PS_Work //5
,e_PS_Work //6
,e_PS_Work //7
,e_PS_Work //8
,e_PS_Work //9
,e_PS_Work //10
,e_PS_Work //11
,e_PS_Work //12
,e_PS_Work //13
,e_PS_Work //14
,e_PS_Work //15	
};
e_PowerState GetNewPowerState(e_FunctionReturnState rstatel);



void SuperLoopACC_init(void)
{
  SLAcc_GoToSleep=false;	
	SLAcc_PowerState=e_PS_Work;
	
	initI2c1();
  I2C_API_INIT();
  BQ25703_DriverReset();
  //while ((get_sysTick_ms()-time) <500) {;};
};


/**
\brief control sleep state


*/
void SuperLoopACC(void)
{ //e_FunctionReturnState returnstate;
  e_FunctionReturnState rstatel;
	e_PowerState SLAcc_PowerStateL;
  //returnstate=e_FRS_Processing;

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
             {maintaskstate=16;};
					if (e_FRS_DoneError==rstatel)
             {maintaskstate=16;};//	 
					// } while ((e_FRS_Done!=rstatel)&&(e_FRS_DoneError!=rstatel));
           break;
   case 16: //work or don't mind sleep
          SLAcc_PowerState=GetNewPowerState(rstatel);
					if (e_PS_ReadySleep!=SLAcc_PowerState) 
						{maintaskstate=15;}   //work or don't mind sleep
					else
            { //off i2c           //sleep don't mind sleep
						  maintaskstate=17; 
							//off 25703
							B_ACC_PinsOnOff(DISABLE);
						
						};
		 break;
	 case 17://ready
		    SLAcc_PowerState=GetNewPowerState(rstatel);
				if (e_PS_ReadySleep!=SLAcc_PowerState)
				{	// on i2c
					maintaskstate=15;
					B_ACC_PinsOnOff(ENABLE);
				};	
		 break;
	 
  default: maintaskstate=0;
  };

};

e_PowerState GetNewPowerState(e_FunctionReturnState rstatel)
{
	uint8_t DataFor_SLAcc_PowerState; // into function?
			    DataFor_SLAcc_PowerState=0;
		      if (SLAcc_GoToSleep ) 										DataFor_SLAcc_PowerState|=(1<<0);
					if (TPSIRQ) 															DataFor_SLAcc_PowerState|=(1<<1);
	        if (e_FRS_DoneError==rstatel)             DataFor_SLAcc_PowerState|=(1<<2);
	        if (!(
						     (e_FSM_ChargeOff==mainFMSstate)
					     ||(e_FSM_RestOff  ==mainFMSstate)
	             )
					   )	
			                              DataFor_SLAcc_PowerState|=(1<<3);
					
					return encoderPowerState[DataFor_SLAcc_PowerState];
					
}

