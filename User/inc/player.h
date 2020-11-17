#ifndef PLAYER_H
#define PLAYER_H

#include "flash.h"
#include "tim3.h"
#include "Spi.h"
#include <math.h>

extern void getFileList(void);
extern void getFile(uint8_t fid);
//void playTimer(void);
extern void calcFreq(void);
extern uint8_t findNextFile(uint8_t fid);
extern uint32_t freqInverse(uint32_t freq);
extern uint16_t getPlayFileSector(int fileInList);
extern void getControlParam(uint16_t fileSect);

//typedef struct{
//	uint16_t playStart		:1;
//	uint16_t playStop			:1;
//	uint16_t durationTime	:1;
//	uint16_t fileEnd			:1;
//}playerFlags;

//extern volatile fileInfo fileList[MAX_FILES_NUM];	//remove VV 6.10.20
//extern volatile playerFlags playFlags;

#endif //PLAYER_H
