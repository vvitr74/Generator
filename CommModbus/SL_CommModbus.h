#ifndef SL_CommModbus_h
#define SL_CommModbus_h

#include <stdbool.h>
#include "BoardSetup.h"

extern int  SL_CommModbus(void);
extern int SL_CommModbusInit(void);

extern systemticks_t USBcommLastTime;
//extern bool USBcommLastTimeSet;

#endif 
