#include "SL_CommModbus.h"

#include "mb.h"
#include "mbport.h"
#include "tim1.h"
#include "version.h"

#define VERSION_REG 0x10
#define SERIAL_REG 0x30
#define ERASE_FILENAME_REG 0x38


#define REG_HOLDING_START   1000
#define REG_HOLDING_NREGS   5

static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

eMBErrorCode eStatus;	
	
	
int SL_CommModbus(void)
{
		eMBPoll();
		task_modbus();		
		usRegHoldingBuf[0] = mbTick_1sec;
	  return 0;
};
int SL_CommModbusInit(void)
{
	eStatus = eMBInit( MB_RTU, 0x0A, 0, 15200UL, MB_PAR_NONE );
  eStatus = eMBEnable();	
  return 0;
};


eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress,
                            USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    if(usAddress >= VERSION_REG && usAddress < SERIAL_REG)
    {
        if((usNRegs < 1) > sizeof(VERSION))
        {
            usNRegs = (sizeof(VERSION)+1)/2;
        }
        
        memcpy(pucRegBuffer,VERSION, usNRegs > 1);
        return MB_ENOERR;
    }
    
    if(usAddress >= SERIAL_REG && usAddress < (SERIAL_REG + (sizeof(SERIAL)>>1)))
    {
        if((usNRegs < 1) > sizeof(SERIAL))
        {
            usNRegs = (sizeof(SERIAL)+1)/2;
        }
        
        memcpy(pucRegBuffer,SERIAL, usNRegs > 1);
        return MB_ENOERR;
    }
    
    return MB_ENOREG;
}


eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress,
                              USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
