#include "SuperLoop_Comm2.h"

#include "SL_CommModbus.h"
extern int spiffs_init();

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
\brief Map e_SLD_FSM onto e_PowerState

e_PS_Work,e_PS_DontMindSleep,e_PS_ReadySleep
*/
const e_PowerState SLC_Encoder[SLC_FSM_NumOfEl]=
{e_PS_Work						//SLC_FSM_InitialWaitSupply
,e_PS_Work						//SLC_FSM_Init
,e_PS_Work						//SLC_FSM_CommAbsent
,e_PS_Work						//SLC_FSM_OnTransitionOffPlayer
,e_PS_Work						//SLC_FSM_USBCommunication 
,e_PS_DontMindSleep		//SLC_FSM_AndroidConnected	
,e_PS_DontMindSleep		//SLC_FSM_WakeTransition
,e_PS_ReadySleep			//SLC_FSM_WakeTransition
};

static e_SLC_FSM state_inner;

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
extern void SLC(void)
{

	//systemticks_t SLD_LastButtonPress;
	switch (state_inner)
	{
		case SLC_FSM_InitialWaitSupply: // initial on
			if (bVSYS) {state_inner=SLC_FSM_Init;};
			break;
		case SLC_FSM_Init:
			//spiffs_init();
		 // SL_CommModbusInit();
			break;
/*		case SLD_FSM_OnTransition: //on transition
				PM_OnOffPWR(PM_Display,true );
				SLD_DisplInit();
//		    gwinRedrawDisplay(NULL,true);
		    state_inner=SLD_FSM_On;
//   break;
		case SLD_FSM_On: // on
#ifdef def_debug_AccDispay
	    	SLDwACC();
#else
		    SLDw();
#endif		
  		if ((!bVSYS)|button_sign)
			{
				button_sign=0;
				state_inner=SLD_FSM_OffTransition;
			};
			break;
		case SLD_FSM_OffTransition: 
      	SLD_DisplDeInit();               //off transition
        PM_OnOffPWR(PM_Display,false );				
				state_inner=SLD_FSM_Off;
		  break;	
		case SLD_FSM_DontMindSleep:
			  SLD_LastButtonPress=BS_LastButtonPress;
		    if (SLD_GoToSleep) 
						state_inner=SLD_FSM_SleepTransition;
				if (((SystemTicks-SLD_LastButtonPress)<SLD_SleepDelay)) 
						state_inner=SLD_FSM_Off;  //has more priority
			break;
		case SLD_FSM_SleepTransition:// sleep transition
		  //reset interrupt pending
		  PM_ClearPendingButton;
		  state_inner=SLD_FSM_Sleep; 
		  //break;
		case SLD_FSM_Sleep:
			//SLD_PowerState= e_PS_ReadySleep;
//			SLD_PWR_State=	false;		
        SLD_LastButtonPress=BS_LastButtonPress;
				if ((!SLD_GoToSleep) || ((SystemTicks-SLD_LastButtonPress)<SLD_SleepDelay)) 
				{state_inner=SLD_FSM_WakeTransition;
				};
		    
			break;
		case SLD_FSM_WakeTransition: //wake transition
		  state_inner=SLD_FSM_Off;
		break;
*/    default: state_inner=SLC_FSM_InitialWaitSupply;		
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

