#ifndef _board_PowerModes_H
#define _board_PowerModes_H

#include "mainFSM.h"
#include "superloopDisplay.h"
#include "superloop_Player.h"

extern void SuperLoop_PowerModes_Init(void);
extern void SuperLoop_PowerModes(void);

extern e_FunctionReturnState  MainTransition_P_Displ(e_FSMState_SuperLoopDisplay state_Displ_new,e_FSMState_SuperLoopDisplay state_Displ_old);

extern e_FunctionReturnState  MainTransition_P_Pl(e_FSMState_SuperLoopPlayer state_Pl_new,e_FSMState_SuperLoopPlayer state_Pl_old);


#endif

