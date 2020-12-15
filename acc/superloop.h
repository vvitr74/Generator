/*
 * superloop.h
 *
 *  Created on: Feb 13, 2019
 *      Author: 2382
 */

#ifndef SUPERLOOP_H_
#define SUPERLOOP_H_

#include <stdbool.h>
#include "PowerModes_Defs.h"

//----------------------------------for main---------------------------------------------
extern void SuperLoopACC(void);
extern void SuperLoopACC_init(void);

//---------------------------------for power---------------------------------------------
typedef enum  {e_AS_NoSysPwr,e_AS_IsSysPwr,e_AS_NumOfel} e_Acc_State;

extern __inline e_PowerState SLAcc_GetPowerState(void);
extern __inline e_PowerState SLAcc_SetSleepState(bool state);
 

#endif /* SUPERLOOP_H_ */
