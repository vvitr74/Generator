#include "flash.h"
#include "tim3.h"
#include "Spi.h"
#include <math.h>
#include "uart.h"

//volatile fileInfo fileList[MAX_FILES_NUM];
uint32_t nonClear;

extern volatile struct{
	uint16_t buffSel					:1;
	uint16_t getStartAddr			:1;
	uint16_t buff0DataRdy			:1;
	uint16_t buff1DataRdy			:1;
	uint16_t firstPage				:1;
	uint16_t lastPage					:1;
}usbFlags;

uint8_t usbRxBuff0[PAGE_SIZE];
uint8_t usbRxBuff1[PAGE_SIZE];
uint32_t rxIrqCnt;
uint16_t startPageAddr;
uint16_t filePagesCnt;
uint8_t lastPageBytesNum;
uint16_t filePagesNum;
uint32_t fileBytesNum;
uint8_t usbCmd;
extern uint32_t playParamArr[7];
//extern uint32_t playFreqArr[200];

uint16_t findEmptySector(void)
{
	uint32_t sectAddr=0;
	while(!(W25qxx_IsEmptySector(sectAddr,0))){
		sectAddr++;
	}
	return sectAddr;
}

void wrPage(void)
{
//	uint8_t size[4];
//	if(rxIrqCnt==3){
//		usbFlags.firstPage=1;
//	}
	if(usbFlags.lastPage==1){
		if(usbFlags.buffSel==0){
			spi1FifoClr();
			W25qxx_WritePage(usbRxBuff0,startPageAddr+filePagesNum-1,0,lastPageBytesNum);
		}
		else{
			spi1FifoClr();
			W25qxx_WritePage(usbRxBuff1,startPageAddr+filePagesNum-1,0,lastPageBytesNum);
		}
		usbFlags.lastPage=0;
		usbFlags.buff0DataRdy=0;
		usbFlags.buff1DataRdy=0;
		usbFlags.getStartAddr=0;
//		usbFlags.buffSel=0;
//		size[0]=(uint8_t)((fileBytesNum & 0xFF000000)>>24);
//		size[1]=(uint8_t)((fileBytesNum & 0xFF0000)>>16);
//		size[2]=(uint8_t)((fileBytesNum & 0xFF00)>>8);
//		size[3]=(uint8_t)(fileBytesNum & 0xFF);
//		spi1FifoClr();
//		W25qxx_WritePage(size,startPageAddr,0,sizeof(size));
		rxIrqCnt=0;
		usbCmd=0;
		endCmd();
		return;
	}
	if(usbFlags.buff0DataRdy==1){
		spi1FifoClr();
		W25qxx_WritePage(usbRxBuff0,startPageAddr+filePagesCnt-1,0,PAGE_SIZE);
//		if(usbFlags.firstPage==1){
//			spi1FifoClr();
//			W25qxx_WritePage(usbRxBuff0,startPageAddr,10,PAGE_SIZE-10);
//			usbFlags.firstPage=0;
//		}
//		else{
//			spi1FifoClr();
//			W25qxx_WritePage(usbRxBuff0,startPageAddr+filePagesCnt-1,0,PAGE_SIZE);
//		}
		usbFlags.buff0DataRdy=0;
		return;
	}
	if(usbFlags.buff1DataRdy==1){
		spi1FifoClr();
		W25qxx_WritePage(usbRxBuff1,startPageAddr+filePagesCnt-1,0,PAGE_SIZE);
//		if(usbFlags.firstPage==1){
//			spi1FifoClr();
//			W25qxx_WritePage(usbRxBuff1,startPageAddr,10,PAGE_SIZE-10);
//			usbFlags.firstPage=0;
//		}
//		else{
//			spi1FifoClr();
//			W25qxx_WritePage(usbRxBuff1,startPageAddr+filePagesCnt-1,0,PAGE_SIZE);
//		}
		usbFlags.buff1DataRdy=0;
		return;
	}
}


void wrPlayFiles(void)
{
	if(usbFlags.getStartAddr==0){
		startPageAddr=W25qxx_SectorToPage(findEmptySector());
		usbFlags.getStartAddr=1;
	}
//	while(usbCmd==0x32){
//		rdPage();
		wrPage();
//	}
}

void rdFlash(void)
{
	uint8_t byte;
	
	for(int i=0;i<SECTOR_SIZE*SECTORS_NUM;i++)
	{
		spi1FifoClr();
		W25qxx_ReadSector(&byte,(uint32_t)i,(uint32_t)i,1);
		USART1->TDR = byte;
		while(!(USART1->ISR & USART_ISR_TC)){}
	}
	usbCmd=0;
}

//void rdPlayFiles(void)
//{
//	uint16_t shift;
//	uint8_t byte;
////	getFileList();
//	for(uint8_t i=0;i<MAX_FILES_NUM;i++){
//		if(fileList[i].fileSize!=0){
////			rdPlayFile(i);
//			for(shift=10;shift<fileList[i].fileSize;shift++){
//				spi1FifoClr();
//				W25qxx_ReadSector(&byte,(uint32_t)i,(uint32_t)shift,1);
//				USART1->TDR = byte;
//				while(!(USART1->ISR & USART_ISR_TC)){}
//			}
//		}
//	}
//	rxIrqCnt=0;
//	usbCmd=0;
//	endCmd();
//}

void wrConfFile(void)
{
	if(usbFlags.getStartAddr==0){
		startPageAddr=W25qxx_SectorToPage(FIRST_CONF_SECT);
		usbFlags.getStartAddr=1;
	}
	wrPage();
}

void rdConfFile(void)
{
//	uint32_t bytesCnt;
//	uint8_t byte;
//	if(!W25qxx_IsEmptySector(FIRST_CONF_SECT,0)){
//		for(bytesCnt=FIRST_CONF_BYTE+10;bytesCnt<(FIRST_CONF_BYTE+CONF_FILE_SIZE+10);bytesCnt++){
//			spi1FifoClr();
//			W25qxx_ReadByte(&byte,bytesCnt);
//			USART1->TDR = byte;
//			while(!(USART1->ISR & USART_ISR_TC)){}
//		}
//	}
//	rxIrqCnt=0;
//	usbCmd=0;
//	endCmd();
}

//void getFileList(void)
//{
//	uint8_t size[4];
//	uint8_t sect=0;
//	for(sect=0;sect<MAX_FILES_NUM;sect++){
//		fileList[sect].fileId=sect;
//		if(!W25qxx_IsEmptySector(sect,0)){
//			spi1FifoClr();
//			W25qxx_ReadSector((uint8_t*)fileList[sect].fileName,sect,FILE_NAME_SHIFT,FILE_NAME_BYTES);
//			spi1FifoClr();
//			W25qxx_ReadSector(size,sect,0,sizeof(size));
//			fileList[sect].fileSize = (uint32_t)((size[0]<<24) | (size[1]<<16) | (size[2]<<8) | size[3]);
//		}
//		else{
//			for(int i=0;i<FILE_NAME_BYTES;i++){
//				fileList[sect].fileName[i]='_';
//			}
//			fileList[sect].fileSize=0;
//		}
//	}
//}


//void setFile(uint8_t fid)
//{
//	uint8_t temp;
//	uint8_t tempArr[6];
//	uint16_t byteCnt=0;
//	uint8_t strCnt=0;
//	uint8_t chrCnt=0;
//	uint32_t startAddr=fid*SECTOR_SIZE;
//	
//	do{																							//skip first line	
//		W25qxx_ReadByte(&temp,startAddr+byteCnt+10);
//		byteCnt++;
//	}while(temp!='\n');
//	
//	while(strCnt<7){																//fill an array of parameters
//		W25qxx_ReadByte(&temp,startAddr+byteCnt+10);
//		byteCnt++;
//		if((temp>='0')&&(temp<='9')){
//			tempArr[chrCnt]=temp;
//			chrCnt++;
//			continue;
//		}
//		if(temp=='\n'){
//			for(int i=0;i<chrCnt;i++){
//				playParamArr[strCnt]+=(uint32_t)(tempArr[i]&0x0F)*(uint32_t)powf(10,chrCnt-1-i);
//				tempArr[i]=0;
//			}
//			chrCnt=0;
//			strCnt++;
//			continue;
//		}
//	}
//	strCnt=0;
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
//				switch(playParamArr[6]){
//					case 0:									//inverse=0
//						playFreqArr[strCnt]+=(uint32_t)(tempArr[i]&0x0F)*(uint32_t)powf(10,chrCnt-1-i);
//						tempArr[i]=0;
//						break;
//					case 1:									//inverse=1
//						if((tempArr[i]&0x0F)!=0){
//							tempArr[i]=10-(tempArr[i]&0x0F);
//						}
//						else{
//							tempArr[i]=0;
//						}
//						playFreqArr[strCnt]+=(uint32_t)tempArr[i]*(uint32_t)powf(10,chrCnt-1-i);
//						break;
//					}
//				}
//			chrCnt=0;
//			strCnt++;
//			continue;
//		}
//	}
//}


void eraseFlash(void)
{
	W25qxx_EraseChip();
	nonEmptyBytes=isFlashClear();
}

//void erFlash(uint8_t firstSectAddr, uint8_t lastSectAddr)
//{
//	if(firstSectAddr<lastSectAddr){
//		for(int i=firstSectAddr;i<=lastSectAddr;i++){
//			spi1FifoClr();
//			W25qxx_EraseSector(i);
//		}
//	}
//	if(firstSectAddr==lastSectAddr){
//		spi1FifoClr();
//		W25qxx_EraseSector(firstSectAddr);
//	}
//	if(firstSectAddr>lastSectAddr){
//		errorCmd();
//	}
//	rxIrqCnt=0;
//	usbCmd=0;
//	endCmd();
//}


void procCmdFromUsb(void)
{
//	usbCmdDetect();
	switch(usbCmd){
		case WR_CONF_FILE:
			wrConfFile();
//			usbCmd=0;
			return;
		case WR_PLAY_FILES:
			wrPlayFiles();
			return;
		case RD_CONF_FILE:
//			rdConfFile();
			rdFlash();
			return;
		case RD_PLAY_FILES:
//			rdPlayFiles();
			rdFlash();
			return;
		case ER_CONF_FILE:
//			erFlash(FIRST_CONF_SECT,LAST_CONF_SECT);
			eraseFlash();
			usbCmd=0;
			return;
		case ER_PLAY_FILES:
//			erFlash(FIRST_PLAY_SECT,LAST_PLAY_SECT);
			eraseFlash();
			usbCmd=0;
			return;
		default:
			return;
	}
//	usbCmd=0;
}

void endCmd(void)
{
	uint8_t report[]="Command complete\r\n";
	for(int i=0;i<sizeof(report)-1;i++){
		USART1->TDR=report[i];
		while(!(USART1->ISR & USART_ISR_TC)){}
	}
}

void errorCmd(void)
{
	uint8_t report[]="Command failed\r\n";
	for(int i=0;i<sizeof(report)-1;i++){
		USART1->TDR=report[i];
		while(!(USART1->ISR & USART_ISR_TC)){}
	}
}

uint32_t isFlashClear(void)
{
	uint32_t bugs=0;
	uint8_t byte;
	
	for(int i=0;i<2097152;i++){
		W25qxx_ReadByte(&byte,i);
		if(byte!=0xFF){
			bugs++;
		}
	}
	return bugs;
}
