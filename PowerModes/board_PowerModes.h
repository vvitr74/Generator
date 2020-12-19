#ifndef _board_PowerModes_H
#define _board_PowerModes_H

#include "mainFSM.h"
#include "superloopDisplay.h"
#include "superloop_Player.h"

extern void SuperLoop_PowerModes_Init(void);
extern void SuperLoop_PowerModes(void);

extern void PM_OnOffPWR_D(bool newstate);
extern void PM_OnOffPWR_Pl(bool newstate);

#endif

