#ifndef _board_PowerModes_H
#define _board_PowerModes_H

#include "mainFSM.h"
#include "superloopDisplay.h"
#include "superloop_Player.h"

extern void SuperLoop_PowerModes_Init(void);
extern void SuperLoop_PowerModes(void);

extern e_FunctionReturnState  MainTransition_P_Displ(bool PWR_state_new,bool PWR_state_old);

extern e_FunctionReturnState  MainTransition_P_Pl(bool PWR_state_new,bool PWR_state_old);


#endif

