#include "BoardSetup.h"
#include "SuperLoop_Comm2.h"
#include "SL_CommModbus.h"

#include "BQ28z610_Data.h"

//------------------------------------for iteraction with MOFBUS
#define USBcommPause 1000

static systemticks_t USBcommLastTimel;
//---------------------------------------------------------------------------------------------
extern int spiffs_init();

// ----------------------------------------for power-------------------------------------------

typedef enum  
{SLC_FSM_InitialWaitSupply  		//work
,SLC_FSM_Init  						      //work
,SLC_FSM_CommAbsent 		        //e_PS_DontMindSleep
,SLC_FSM_OnTransitionOffPlayer 	//work
,SLC_FSM_USBCommunication		    //e_PS_DontMindSleep
,SLC_FSM_AndroidConnected 	    //work
,SLC_FSM_Sleep           	      //  ready for sleep
,SLC_FSM_WakeTransition  	      //work
,SLC_FSM_NumOfEl	
} e_SLC_FSM;

/**
\brief Map e_SLC_FSM onto e_PowerState

e_PS_Work,e_PS_DontMindSleep,e_PS_ReadySleep
*/
const e_PowerState SLC_Encoder[SLC_FSM_NumOfEl]=
{e_PS_Work						//SLC_FSM_InitialWaitSupply
,e_PS_Work						//SLC_FSM_Init
,e_PS_DontMindSleep		//SLC_FSM_CommAbsent
,e_PS_Work						//SLC_FSM_OnTransitionOffPlayer
,e_PS_Work						//SLC_FSM_USBCommunication 
,e_PS_Work	        	//SLC_FSM_AndroidConnected	
,e_PS_ReadySleep			//SLC_FSM_Sleep
,e_PS_Work						//SLC_FSM_WakeTransition
};

const bool SPIFFS_ReadyEncoder[SLC_FSM_NumOfEl]=
{false						//SLC_FSM_InitialWaitSupply
,false						//SLC_FSM_Init
,true		          //SLC_FSM_CommAbsent
,false						  //SLC_FSM_OnTransitionOffPlayer
,false					  	//SLC_FSM_USBCommunication 
,false	          	//SLC_FSM_AndroidConnected	
,false		      	//SLC_FSM_Sleep
,false						//SLC_FSM_WakeTransition
};

const bool SLC_BusyEncoder[SLC_FSM_NumOfEl]=
{false						//SLC_FSM_InitialWaitSupply
,false						//SLC_FSM_Init
,false		          //SLC_FSM_CommAbsent
,false						  //SLC_FSM_OnTransitionOffPlayer
,true					  	//SLC_FSM_USBCommunication 
,true	          	//SLC_FSM_AndroidConnected	
,false		      	//SLC_FSM_Sleep
,false						//SLC_FSM_WakeTransition
};




static e_SLC_FSM state_inner;
//static bool USBcomm;
static bool OffPlayer=true; //rdd debug

__inline bool SLC_SPIFFS_State(void)
{
	 return SPIFFS_ReadyEncoder[state_inner];
};
__inline bool SLC_Busy_State(void)
{
	 return SLC_BusyEncoder[state_inner];
};


//---------------------------------for power sleep---------------------------------------------
//static e_PowerState SLD_PowerState; 
static bool SLC_GoToSleep;

__inline e_PowerState SLC_GetPowerState(void)
{
	 return SLC_Encoder[state_inner];
};

__inline e_PowerState SLC_SetSleepState(bool state)
{
	SLC_GoToSleep=state;
	return SLC_Encoder[state_inner];
};



extern void SLC_init(void)
{

};

//#define SL_CommModbus()

extern void SLC(void)
{

	//systemticks_t SLD_LastButtonPress;
	if ((!bVSYS))
		state_inner=SLC_FSM_InitialWaitSupply;
	switch (state_inner)
	{
		case SLC_FSM_InitialWaitSupply: // initial on
			if (bVSYS) 
      {state_inner=SLC_FSM_Init;};
			break;
		case SLC_FSM_Init:
			PM_OnOffPWR(PM_Communication,true );
			spiffs_init();
		  SL_CommModbusInit();
		
//		readDataFromFile();	//for debug
		
		  state_inner=SLC_FSM_CommAbsent;
			break;
		case SLC_FSM_CommAbsent: //
			  SL_CommModbus();
				if (SLC_GoToSleep)
					  PM_OnOffPWR(PM_Communication,false);
				USBcommLastTimel=USBcommLastTime; //not volatile
				if ((SystemTicks-USBcommLastTimel)>(2*USBcommPause))
				   {   USBcommLastTime=SystemTicks-(2*USBcommPause);
					 }	 

				if ((SystemTicks-USBcommLastTimel)<USBcommPause)
					  state_inner=SLC_FSM_OnTransitionOffPlayer;
//				if (Bluetooth)
//					  state_inner=SLC_FSM_OnTransitionOffPlayer;
      break;
		case SLC_FSM_OnTransitionOffPlayer: // on
			SL_CommModbus();
  		if (OffPlayer)
			{
				state_inner=SLC_FSM_USBCommunication;
			};
			break;
		case SLC_FSM_USBCommunication: 
			SL_CommModbus();
      if ((SystemTicks-USBcommLastTimel)>(USBcommPause))				
				  state_inner=SLC_FSM_CommAbsent;
		  break;	
		case SLC_FSM_AndroidConnected:
			break;
		case SLC_FSM_Sleep:
				if ((!SLC_GoToSleep) ) 
	  			{state_inner=SLC_FSM_WakeTransition;
				  };
			break;
		case SLC_FSM_WakeTransition: //wake transition
		  state_inner=SLC_FSM_CommAbsent;
		break;
    default: state_inner=SLC_FSM_InitialWaitSupply;		
	};
	//return 0;
};


void Communication_InSleep()
{
	RCC->APBENR2 |= RCC_APBENR2_USART1EN;
	USART1->CR1 &= ~USART_CR1_UE;
	RCC->APBENR2 &= ~RCC_APBENR2_USART1EN;
};
void Communication_OutSleep()
{
  //uart1Init();	
};

