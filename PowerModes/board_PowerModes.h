#ifndef _board_PowerModes_H
#define _board_PowerModes_H

#include "mainFSM.h"
#include "superloopDisplay.h"
#include "superloop_Player.h"


//moduls
#define PM_Display 2
#define PM_Player 1
#define PM_Communication 4

extern void PM_OnOffPWR(uint8_t modul, bool newstate);
	
extern void SuperLoop_PowerModes_Init(void);
extern void SuperLoop_PowerModes(void);

extern void PM_OnOffPWR(uint8_t modul, bool newstate);

#endif

