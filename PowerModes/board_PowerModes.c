#include "stm32g0xx.h"
#include <stdbool.h>
#include "board_PowerModes.h"
#include "BoardSetup.h"
#include "superloop.h"
#include "superloopDisplay.h"
#include "superloop_Player.h"

#define SLP_WakeUpPause 500




static systemticks_t SLP_LastUpdateTime;
static uint8_t SLP_state;
static bool SLP_sleep;
static bool SLP_WakeUP;

/*************************************************************************************************************************
*
*
**************************************************************************************************************************/
void EXTI4_15_IRQHandler(void){
	
	uint32_t risingEdge = EXTI->RPR1;										// get all rising EXTI flags
	uint32_t failingEdge = EXTI->FPR1;									// get all rising EXTI flags
	
	EXTI->RPR1 = risingEdge;														// clear all rising EXTI flags					
  
  if(risingEdge & EXTI_RPR1_RPIF13){            			// if EXTI13 interrupt flag set (TOUCH_DETECT)
	//	statusFlag.TOUCH_DETECT = 1;
	}
	
	if(risingEdge & EXTI_RPR1_RPIF5){            				// if EXTI5 interrupt flag set	(BUTTON PUSH)		
		EXTI->IMR1 &= ~(EXTI_IMR1_IM5 | EXTI_IMR1_IM7);  	// EXTI interrupt masked
	}
	
	if(failingEdge & EXTI_FPR1_FPIF7){            			// if EXTI7 interrupt flag set (VBUS_DETECT)
		EXTI->IMR1 &= ~(EXTI_IMR1_IM5 | EXTI_IMR1_IM7);   // EXTI interrupt masked
	}
}
//***********************************************************************************************************************



void enterToStop(void);

void SuperLoop_PowerModes_Init(void)
	{
		
	}		
	
	
void SuperLoop_PowerModes(void)
	{
		bool rplayer;
		bool rdispl;
		bool racc;
		switch (SLP_state)
		{
			case 0://work mode
				if (!SLP_sleep)
					{break;
					}
				SLP_state=1;
			case 1:	
    		rplayer=SuperLoop_Player_SleepIn();
    		rdispl=SuperLoop_Disp_SleepIn();
	    	racc=SuperLoop_Acc_SleepIn();
    		if (rdispl && rplayer)
					{ 
//            enterToStop();
						SLP_sleep=false;
      		};
		    SuperLoop_Acc_SleepOut();
		    SuperLoop_Disp_SleepOut();
	    	SuperLoop_Player_SleepOut();		
				SLP_LastUpdateTime=SystemTicks;	
				SLP_state=2;	
				//break;	
			case 2: //WakeUp?
			
				 if (SLP_WakeUP)
				  {
						SLP_state=0;
						SLP_WakeUP=false;
						break;
				  }
//				if (SLP_sleep)
//					{
//						SLP_state=1;
//				    SLP_sleep=false;
//						break;
//					};	
			   if (!(SLD_FSMState()||SLPl_FSMState()))
				    { 
				  	 SLP_state=0;
						 break;
				    }
						
				if (SLP_WakeUpPause<(SystemTicks-SLP_LastUpdateTime))
					{
					  SLP_state=1;
						break;
          };
					break;
			default: 	SLP_state=0;
	   };
	 }		

void enterToStop(void)
{
  GPIOB->BSRR = GPIO_BSRR_BS10;
	EXTI->EXTICR[2] &= ~(EXTI_EXTICR2_EXTI5_Msk );					// select PA5 for EXTI
  EXTI->RTSR1 |= EXTI_RTSR1_RT5;                        	// set EXTI5 trigger to rising front
	
//	EXTI->FPR1 = EXTI_FPR1_FPIF7;       										// clear EXTI7, EXTI7 interrupt flags
//	EXTI->IMR1 |= EXTI_IMR1_IM7;                            // EXTI7  interrupts unmasked
	EXTI->RPR1 = EXTI_RPR1_RPIF5;                             // clear EXT5, EXT5 interrupt flags
	EXTI->IMR1 |= EXTI_IMR1_IM5;          	                  // EXTI5 interrupts unmasked
	
	PWR->CR1 |= PWR_CR1_LPR | 															// the regulator is switched from main mode (MR) to low-power mode
						  PWR_CR1_LPMS_0;															// select Stop 1 low-power mode

	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; 											// Set SLEEPDEEP bit of Cortex System Control Register 

	__WFI();
	
			/* wakeup */
	
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk; 										// reset SLEEPDEEP bit of Cortex System Control Register																
	PWR->CR1 &= ~(PWR_CR1_LPMS_Msk | PWR_CR1_LPR);					// the regulator is switched from low-power mode to main mode (MR)

	SystemCoreClock = setSystemClock(); 									// restore all setting after stop
  SysTick_Config(SystemCoreClock/1000);


	GPIOB->BSRR = GPIO_BSRR_BR10;
}	 
	 
	 
typedef uint32_t key_type;
	
typedef
   struct { 
			key_type mFSM_Error;
		  uint8_t state;
    } s_FSM_Data;


		
s_FSM_Data FDMD_Power;
e_FunctionReturnState  FSM_MainTransition_P(s_FSM_Data * pFDMD_Power,key_type key);
e_FunctionReturnState TransitionFunction_P(uint8_t state);		


typedef enum  {e_P_DN,e_P_DY,e_P_PlN,e_P_PlY // 4 interface pins
	,e_P_SPI1N
	,e_P_GLOBAL_ON,e_P_TFT_ON, e_P_UTSTAGE_ON ,e_P_UTSTAGE_OFF  ,e_P_TFT_OFF  ,e_P_GLOBAL_OFF //PWR 4+6=10
	,e_P_SPI1Y //interface 10+2=12
	,e_P_sleep,e_P_wakeup // 12+1=13
,e_P_NumOfel} e_TransitionFunctionType;// 13+1=14

#define Nxx (1<<e_P_DN)   // digital pins and PWR
#define Yxx (1<<e_P_DY) 
#define xNx (1<<e_P_PlN)
#define xYx (1<<e_P_PlY)

#define SPI1N (1<<e_P_SPI1N)

#define Pl_G_On (1<<e_P_GLOBAL_ON)
#define D_P_On  (1<<e_P_TFT_ON)
#define Pl_P_On (1<<e_P_UTSTAGE_ON)
#define Pl_P_Off (1<<e_P_UTSTAGE_OFF)
#define D_P_Off (1<<e_P_TFT_OFF)
#define Pl_G_Off (1<<e_P_GLOBAL_OFF)

#define SPI1Y (1<<e_P_SPI1Y)

#define sleepx (1<<e_P_sleep)
#define wakeupx (1<<e_P_wakeup)

//Display Player
static const key_type PWR2_TransitionKeys[4][4]=  //int a[ROWS][COLS] = 
{//     NN                                                       NY       YN         YY 
/*NN*/{Nxx|xNx|SPI1N|Pl_P_Off|D_P_Off|Pl_G_Off|sleepx,            0,       0,        0  },
/*NY*/{Pl_G_On|D_P_On|Pl_P_On|SPI1Y|wakeupx,                   Yxx|xYx, Pl_P_On,     0  },
/*YN*/{Pl_G_On|D_P_On|SPI1Y|wakeupx,                           Pl_P_Off, Yxx|xNx, Pl_P_Off  },
/*YY*/{Pl_G_On|D_P_On|Pl_P_On|SPI1Y|wakeupx,                      0,        0,    Yxx|xYx},
};

//TransitionKeys[mainFMSstate][mainFMSstate]); 
#define MStateP(d,p) ((((d)<<1)|((p)<<0))&0x3)
e_FunctionReturnState  MainTransition_P_Displ(e_FSMState_SuperLoopDisplay state_Displ_new,e_FSMState_SuperLoopDisplay state_Displ_old)
{
	uint8_t statenew;
	uint8_t stateold;
	e_FSMState_SuperLoopPlayer spl;
//	e_FSMState_SuperLoopDisplay sd;
	spl=SLPl_FSMState();
	stateold=MStateP( state_Displ_old,spl);
	statenew=MStateP( state_Displ_new,spl);
	return FSM_MainTransition_P(&FDMD_Power, PWR2_TransitionKeys[statenew][stateold]);
};

e_FunctionReturnState  MainTransition_P_Pl(e_FSMState_SuperLoopPlayer state_Pl_new,e_FSMState_SuperLoopPlayer state_Pl_old)
{
	uint8_t statenew;
	uint8_t stateold;
	e_FSMState_SuperLoopDisplay sd;
	sd=SLD_FSMState();
	stateold=MStateP( sd,state_Pl_old);
	statenew=MStateP( sd,state_Pl_new);
	return FSM_MainTransition_P(&FDMD_Power, PWR2_TransitionKeys[statenew][stateold]);
};


e_FunctionReturnState  FSM_MainTransition_P(s_FSM_Data * pFDMD_Power,key_type key)
{
//	static uint8_t state=0;
//	static uint8_t state1=0;
	e_FunctionReturnState rstate,rstatel;
	
	rstate=e_FRS_Processing;
while(1)
	{	
		if (((key>>pFDMD_Power->state)&1)!=0)
			{ rstatel=TransitionFunction_P(pFDMD_Power->state);
              if (e_FRS_Done==rstatel)   //switch ?
                                      {pFDMD_Power->state++;};
              if (e_FRS_DoneError==rstatel)
									{ pFDMD_Power->mFSM_Error|=1<<pFDMD_Power->state;  // Tries to do everything regardless of mistakes
										pFDMD_Power->state++;
									};
              if (e_FRS_Processing==rstatel)
								{ break;
								};
	        }
	        else
	        {pFDMD_Power->state++;};
		     if (pFDMD_Power->state>=e_P_NumOfel)  
					{ 
						if (pFDMD_Power->mFSM_Error==0) 
						{rstate=e_FRS_Done;}
						else
						{rstate=e_FRS_DoneError;};
					pFDMD_Power->state=0;
					pFDMD_Power->mFSM_Error=0;
					break;	//exit
					};   
	};
	return rstate;
}

e_FunctionReturnState TransitionFunction_P(uint8_t state)
{   e_FunctionReturnState rstate;
	switch (state)
	{ int currl;
	  case e_P_DN:	 				switchDisplayInterfacePinsToPwr(DISABLE);	rstate=e_FRS_Done; break;//0
	  case e_P_DY: 					switchDisplayInterfacePinsToPwr(ENABLE);	rstate=e_FRS_Done; break;//1
	  case e_P_PlN: 				switchOUTStageInterfacePinsToPwr(DISABLE);rstate=e_FRS_Done; break;//2
	  case e_P_PlY: 				switchOUTStageInterfacePinsToPwr(ENABLE);	rstate=e_FRS_Done; break;//2
		
	  case e_P_SPI1N: 			switchSPI1InterfacePinsToPwr(DISABLE);		rstate=e_FRS_Done; break;//3
		
		case e_P_GLOBAL_ON:   PWR_GLOBAL_ON;														rstate=e_FRS_Done; break;
		case e_P_TFT_ON:			PWR_TFT_ON;																rstate=e_FRS_Done; break;
		case e_P_UTSTAGE_ON:	PWR_UTSTAGE_ON;														rstate=e_FRS_Done; break;
		case e_P_UTSTAGE_OFF:	PWR_UTSTAGE_OFF;													rstate=e_FRS_Done; break;
		case e_P_TFT_OFF:			PWR_TFT_OFF;															rstate=e_FRS_Done; break;
		case e_P_GLOBAL_OFF:	PWR_GLOBAL_OFF;														rstate=e_FRS_Done; break;
		
  	case e_P_SPI1Y: 			switchSPI1InterfacePinsToPwr(ENABLE);			rstate=e_FRS_Done; break;//4
		
    case e_P_sleep: SLP_sleep=true ;	SLP_WakeUP=false;									rstate=e_FRS_Done; break;
    case e_P_wakeup:SLP_sleep=false ;	SLP_WakeUP=true;									rstate=e_FRS_Done; break;

  	default: 																												rstate=e_FRS_DoneError;
	}
	return rstate;
}




