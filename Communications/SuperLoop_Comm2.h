#ifndef SLC2_H
#define SLC2_H

#include "board_PowerModes.h"
#include "spiffs.h"

void SLC_init(void);
void SLC(void);


e_PowerState SLC_GetPowerState(void);
e_PowerState SLC_SetSleepState(bool state);
bool SLC_SPIFFS_State(void);

void Communication_InSleep();
void Communication_OutSleep();

extern spiffs fs;



#endif
