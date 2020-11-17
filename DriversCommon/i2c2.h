#ifndef __I2C2_H 
#define __I2C2_H

#include "BoardSetup.h"
#include "I2C_COMMON.h"

/* functions prototypes */
extern void initI2c2(void);
extern void i2c2Start(void);
//extern void i2cRepStart(void);
extern uint16_t i2c2DataRW(uint8_t SlaveAddress, transferMode_e tMode, uint16_t subAddr,  uint8_t subAddrLen, uint8_t* pDataBuff, uint8_t len );
//extern void i2cStateMachine(void);
extern uint8_t getI2c2Status(void); //return true(255) if busy
extern uint8_t getI2c2Error(void); //return false (0) if no error

#endif
