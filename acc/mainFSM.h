/*
 * mainFSM.h
 *
 *  Created on: 8 мая 2019 г.
 *      Author: RD
 */

#ifndef MAINFSM_H_
#define MAINFSM_H_

#include <stdint.h>
#include <stdbool.h>

#include "i2c_COMMON.h"

//-------------------------------------------- for disoley-------------------------------------

extern  uint16_t I87;
extern  uint16_t V87;
extern  uint16_t mFSM_BQ28z610_Temperature;
extern  uint16_t pv_BQ28z610_Voltage;
extern  uint16_t mFSM_BQ28z610_RSOC;


extern  uint16_t pvIcharge;
extern  uint16_t pvIdescharge;

extern  int ChargeCurr;
extern  int InCurrent;
//----------------------------------------------------------------------------------------------
//                  0           1          2               3             4            5
typedef enum  {e_FSM_Charge,e_FSM_Rest,e_FSM_ChargeOff,e_FSM_RestOff,e_FSM_Init,e_FSM_NumOfel} e_FSM_State;

extern e_FSM_State mainFMSstate;

extern e_FunctionReturnState mainFSMfunction(void);
extern e_FunctionReturnState ReadTPSState(void);

extern bool bVSYS;


#endif /* MAINFSM_H_ */
