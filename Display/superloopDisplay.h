#ifndef superloopDisplay_h
#define superloopDisplay_h

#include <stdbool.h>
//#include "stm32g0xx.h"
#include "gfx.h"
//#include "SuperLoop_Player.h"
//#include "SuperLoop_Comm.h"
#include "PowerModes_Defs.h"


extern uint8_t spiDispCapture;
extern GHandle	ghList1, ghLabel3, ghLabel4, ghLabel5, ghLabel6, ghLabel7;

// for PowerControl
extern __inline e_PowerState SLD_GetPowerState(void);
extern __inline e_PowerState SLD_SetSleepState(bool state);
extern __inline bool SLD_PWRState(void);


//For main
extern int SLD_init(void);
extern int SLD(void);

#define TFT_CS_H GPIOD->BSRR=GPIO_BSRR_BS3
#define TFT_CS_L GPIOD->BSRR=GPIO_BSRR_BR3

#endif
