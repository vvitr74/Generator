/*
 * battery.c
 *
 *  Created on: 5 мая 2019 г.
 *      Author: RD
 */
#include <stdint.h>
#include "battery.h"

//#define TEMP_MIN (000)
//#define TEMP_REC_MIN (1000)
//#define TEMP_HI_MIN (4000)
//#define TEMP_MAX (4500)


const int BatteryVoltageRanges[]={VOLTAGE_MIN,VOLTAGE_FAST_CHARGE,VOLTAGE_CHARGE_RENEWAL,VOLTAGE_CHARGED};
const int BatteryTempRanges[]={TEMP_MIN,TEMP_REC_MIN,TEMP_HI_MIN,TEMP_MAX};
const int BatteryCurrentSetting[sizeof(BatteryTempRanges)/sizeof (int)+1][sizeof(BatteryVoltageRanges)/sizeof (int)+1]=
{{0,0,0,0,0}
 //  275
,{0.05*BatteryC,0.05*BatteryC,0.1*BatteryC,0.1*BatteryC,0}
  //1375
,{0.25*BatteryC,0.25*BatteryC,0.5*BatteryC,0.5*BatteryC,0}
// 687                         
,{0.125*BatteryC,0.125*BatteryC,0.25*BatteryC,0.25*BatteryC,0}
,{0,0,0,0,0}
};

unsigned char FindRange(int v,const int * a, unsigned char sizeOfArray)
{ unsigned char i;
  for (i=0;i<sizeOfArray;i++)
	if (v<a[i]) break;
  return i;

}

int fChargeCurrent(int Tval,int Vval)
{
	unsigned char Vrange=FindRange(Vval,BatteryVoltageRanges,sizeof(BatteryVoltageRanges)/sizeof (int));//??????
	unsigned char Trange=FindRange(Tval,BatteryTempRanges,sizeof(BatteryTempRanges)/sizeof (int));//??????
	int curr=BatteryCurrentSetting[Trange][Vrange];
	if (BatteryImax<curr) curr=BatteryImax;
	return curr;
}

//e_BatteryState BatteryState=e_BatteryState_Charge;
//int BatteryFSM(int T,int V, int I)
//{ int curr=0;
//  switch (BatteryState)
//  { case e_BatteryState_Charge:
//	     if ((VOLTAGE_CHARGED<V)&&(BatteryIChargeCutOff>I)) BatteryState=e_BatteryState_Charge;
//	     curr=fChargeCurrent(T,V/2);
//	     break;
//    default:
//    	 if (VOLTAGE_CHARGE_RENEWAL>V) BatteryState=e_BatteryState_Charge;
//    	 curr=0;
//  };
//  return curr;
//}






