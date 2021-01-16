#ifndef SLC2_H
#define SLC2_H

#include "board_PowerModes.h"

extern void SLC_init(void);
extern void SLC(void);


extern __inline e_PowerState SLC_GetPowerState(void);
extern __inline e_PowerState SLC_SetSleepState(bool state);

extern void Communication_InSleep();
extern void Communication_OutSleep();

#endif
