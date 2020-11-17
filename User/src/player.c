#include "player.h"
#include "Spi.h"

extern uint32_t playParamArr[7];
/*************************************
playParamArr[0] - frequencies
playParamArr[1] - offset
playParamArr[2] - onset
playParamArr[3] - duration
playParamArr[4] - negative
playParamArr[5] - up
playParamArr[6] - inverse
**************************************/
//extern uint32_t playFreqArr[200];
//uint32_t playFreqTmpArr[200];
GHandle		ghList1;

extern volatile struct{
	uint16_t playStart		:1;
	uint16_t playStop			:1;
	uint16_t durationTime	:1;
	uint16_t fileEnd			:1;
}playerFlags;

void getFileList(void)
{
	uint8_t size[4];
	uint8_t sect=0;
	uint8_t fileName[FILE_NAME_BYTES];
	
	extern int playFileInList;
	uint16_t playFileSector;
	
	for(sect=0;sect<MAX_FILES_NUM;sect++){
		if(!W25qxx_IsEmptySector(sect,0)){
			spi1FifoClr();
			W25qxx_ReadSector((uint8_t*)fileName,sect,FILE_NAME_SHIFT,FILE_NAME_BYTES);
			spi1FifoClr();
			gwinListAddItem(ghList1, (char*)fileName, gTrue);
		}
	}
}

uint16_t getPlayFileSector(int fileInList)
{
	uint16_t sect=0;
	
	for(sect=0;sect<MAX_FILES_NUM;sect++){
		if(!W25qxx_IsEmptySector(sect,0)){
			if(sect==fileInList){
				return sect;
			}
		}
	}
}

void getControlParam(uint16_t fileSect)
{
	uint8_t temp;
	uint8_t tempArr[6];
	uint16_t byteCnt=0;
	uint8_t strCnt=0;
	uint8_t chrCnt=0;
	uint32_t startAddr=fileSect*SECTOR_SIZE;
	
	do{																							//skip first line	
		W25qxx_ReadByte(&temp,startAddr+byteCnt+10);
		byteCnt++;
	}while(temp!='\n');
	
	while(strCnt<7){																//fill an array of parameters
		W25qxx_ReadByte(&temp,startAddr+byteCnt+10);
		byteCnt++;
		if((temp>='0')&&(temp<='9')){
			tempArr[chrCnt]=temp;
			chrCnt++;
			continue;
		}
		if(temp=='\n'){
//			for(int i=0;i<chrCnt;i++){
//				playParamArr[strCnt]+=(uint32_t)(tempArr[i]&0x0F)*(uint32_t)powf(10,chrCnt-1-i);
//				tempArr[i]=0;
//			}
			playParamArr[strCnt]=tempArr[0]*100000U+tempArr[1]*10000U+tempArr[2]*1000U+tempArr[3]*100U+tempArr[4]*10U+tempArr[5];
			chrCnt=0;
			strCnt++;
			continue;
		}
	}
}

uint8_t findNextFile(uint8_t fid)
{
	uint8_t sect=fid;
//remove VV 6.10.20
//	while(fileList[sect].fileSize==0){
//		if(sect==LAST_PLAY_SECT){
//			sect=0;
//		}
//		else{
//			sect++;
//		}
//	}
	return sect;
}

void getFile(uint8_t fid)
{
	uint8_t temp;
	uint8_t tempArr[6];
	uint16_t byteCnt=0;
	uint8_t strCnt=0;
	uint8_t chrCnt=0;
	uint32_t startAddr=fid*SECTOR_SIZE;
		
	do{																							//skip first line	
		W25qxx_ReadByte(&temp,startAddr+byteCnt+10);
		byteCnt++;
	}while(temp!='\n');
	
	while(strCnt<7){																//fill an array of parameters
		W25qxx_ReadByte(&temp,startAddr+byteCnt+10);
		byteCnt++;
		if((temp>='0')&&(temp<='9')){
			tempArr[chrCnt]=temp;
			chrCnt++;
			continue;
		}
		if(temp=='\n'){
//			for(int i=0;i<chrCnt;i++){
//				playParamArr[strCnt]+=(uint32_t)(tempArr[i]&0x0F)*(uint32_t)powf(10,chrCnt-1-i);
//				tempArr[i]=0;
//			}
			playParamArr[strCnt]=tempArr[0]*100000U+tempArr[1]*10000U+tempArr[2]*1000U+tempArr[3]*100U+tempArr[4]*10U+tempArr[5];
			chrCnt=0;
			strCnt++;
			continue;
		}
	}
	strCnt=0;
//	playParamArr[6]=1;															//for debug
//	while(strCnt<playParamArr[0]){									//fill an array of frequencies
//		W25qxx_ReadByte(&temp,startAddr+byteCnt+10);
//		byteCnt++;
//		if((temp>='0')&&(temp<='9')){
//			tempArr[chrCnt]=temp;
//			chrCnt++;
//			continue;
//		}
//		if(temp=='\n'){
//			for(int i=0;i<chrCnt;i++){
////				stringArr[strCnt][i]=tempArr[i];
////				switch(playParamArr[6]){
////					case 0:									//inverse=0
////						playFreqTmpArr[strCnt]+=(uint32_t)(tempArr[i]&0x0F)*(uint32_t)powf(10,chrCnt-1-i);
//						tempArr[i]=0;
////						break;
////					case 1:									//inverse=1
////						if((tempArr[i]&0x0F)!=0){
////							tempArr[i]=10-(tempArr[i]&0x0F);
////						}
////						else{
////							tempArr[i]=0;
////						}
////						playFreqArr[strCnt]+=(uint32_t)tempArr[i]*(uint32_t)powf(10,chrCnt-1-i);
////						break;
////					}
//				}
//			chrCnt=0;
//			strCnt++;
//			continue;
//		}
//	}
}

void calcFreq(void)
{
	static uint8_t steps;
	//Frequency change from (f0+offset) to f0
	if(playParamArr[4]==0 && playParamArr[5]==0){	//negative==0 and up==0
		if(playParamArr[1]==0){
			playerFlags.fileEnd=1;
		}
		else{
			for(int i=0;i<200;i++){
//				playFreqTmpArr[i]+=playParamArr[1];
			}
		}
		playParamArr[1]--;
	}
	//Frequency change from f0 to (f0-offset)
	if(playParamArr[4]==1 && playParamArr[5]==0){	//negative==1 and up==0
		if(steps==playParamArr[1]){
			playerFlags.fileEnd=1;
			steps=0;
		}
		else{
			for(int i=0;i<200;i++){
//				playFreqTmpArr[i]-=steps;
			}
		}
		steps++;
	}
	//Frequency change from f0 to (f0+offset)
	if(playParamArr[4]==0 && playParamArr[5]==1){	//negative==0 and up==1
		if(steps==playParamArr[1]){
			playerFlags.fileEnd=1;
			steps=0;
		}
		else{
			for(int i=0;i<200;i++){
//				playFreqTmpArr[i]+=steps;
			}
		}
		steps++;
	}
	//Validation value inverse
	for(int i=0;i<200;i++){
		if(playParamArr[6]==1){
//			playFreqArr[i]=freqInverse(playFreqTmpArr[i]);
		}
		else{
//			playFreqArr[i]=playFreqTmpArr[i];
		}
	}
}

uint32_t freqInverse(uint32_t freq)
{
	uint8_t tempArr[6];
	uint32_t remain;
	
	tempArr[5]=freq/100000U;
	remain=freq%100000U;
	tempArr[4]=remain/10000U;
	remain=remain%10000U;
	tempArr[3]=remain/1000U;
	remain=remain%1000U;
	tempArr[2]=remain/100U;
	remain=remain%100U;
	tempArr[1]=remain/10U;
	remain=remain%10U;
	tempArr[0]=remain;
	for(int i=0;i<6;i++){
		if(tempArr[i]!=0)
			tempArr[i]=10-tempArr[i];
	}
	return tempArr[5]*100000U+tempArr[4]*10000U+tempArr[3]*1000U+tempArr[2]*100U+tempArr[1]*10U+tempArr[0];
}
