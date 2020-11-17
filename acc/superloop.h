/*
 * superloop.h
 *
 *  Created on: Feb 13, 2019
 *      Author: 2382
 */

#ifndef SUPERLOOP_H_
#define SUPERLOOP_H_

#include <stdbool.h>

//----------------------------------for main---------------------------------------------
extern void SuperLoopACC(void);
extern void SuperLoopACC_init(void);

//---------------------------------for power---------------------------------------------
typedef enum  {e_AS_NoSysPwr,e_AS_IsSysPwr,e_AS_NumOfel} e_Acc_State;
bool SuperLoop_Acc_SleepIn(void);
bool SuperLoop_Acc_SleepOut(void);
//extern __inline e_FSM_State SLAcc_FSMState(void); 

#endif /* SUPERLOOP_H_ */
