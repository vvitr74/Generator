#include "BQ28z610_Data.h"

#define D_ParamStringLength 90
#define StringsNum 118
#define DataBytesNum 40

char byteBuff[D_ParamStringLength];
char tempArrOld[D_ParamStringLength];
uint8_t dataArr[DataBytesNum];
uint16_t dataAddr;
static int8_t bytesCount;
static	uint8_t n_for_CR;
uint8_t bytesNum;
e_FunctionReturnState wrstate;

s32_t data_file;

e_FunctionReturnState readDataFromFile(void)
{
	uint8_t ch, chCnt, strCnt;
	
	char file[]="bq28z610.srec";
	e_FunctionReturnState rstate=e_FRS_Done;
	char *pch;
	int32_t TempParam;
	char byteBuff[D_ParamStringLength+1];
	
	data_file=SPIFFS_open(&fs,file,SPIFFS_O_RDONLY,0);
	
//	chCnt=0;
	strCnt=0;
	n_for_CR=0;
	tempArrOld[0]=0;
	
	do{    
    		bytesCount=SPIFFS_read(&fs, data_file, &byteBuff, D_ParamStringLength-n_for_CR);
				if (bytesCount<0)
				{	rstate=e_FRS_DoneError;
					break;
				}
				byteBuff[bytesCount]=0;
				strcat(tempArrOld,byteBuff);
				dataProcessing();
				do{
					wrstate=BQ28z610_AltManufacturerAccessDFWrite(dataAddr,dataArr,bytesNum-5,0);
				}while(wrstate!=e_FRS_Done || wrstate!=e_FRS_DoneError);
        pch = strchr(tempArrOld,10);
				if (NULL==pch)
				{	rstate=e_FRS_DoneError;
					break;
				}	
				strcpy(tempArrOld,pch+1);	
				n_for_CR= strlen(tempArrOld);
				strCnt++;
	}while (strCnt<=StringsNum);	
	SPIFFS_close(&fs,data_file);
	return rstate=wrstate;
}

void dataProcessing(void)
{
	uint8_t temp[2];
//	uint8_t bytesNum;
	uint8_t index;
	
	bytesNum=char2hex(tempArrOld[2])*16+char2hex(tempArrOld[3]);
	dataAddr=char2hex(tempArrOld[8])*4096+char2hex(tempArrOld[9])*256+char2hex(tempArrOld[10])*16+char2hex(tempArrOld[11]);
	index=12;
	for(int i=0;i<(bytesNum-5);i++){
		dataArr[i]=char2hex(tempArrOld[index])*16+char2hex(tempArrOld[index+1]);
		index+=2;
	}
}

uint8_t char2hex(char ch)
{
	if(ch>='0' && ch<='9')
		return ch & 0x0F;
	switch(ch){
		case 'A':
			return 0x0A;
		case 'B':
			return 0x0B;
		case 'C':
			return 0x0C;
		case 'D':
			return 0x0D;
		case 'E':
			return 0x0E;
		case 'F':
			return 0x0F;
		default:
			return 0;
	}
}
