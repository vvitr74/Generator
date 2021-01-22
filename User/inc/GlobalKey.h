#ifndef GlobalKey_h
#define GlobalKey_h

#define PowerUSE
#define LCDUSE
#define ACCUSE
#define COMM2  //SPIFFS + MODBUS
#define PLAYER

#if defined COMM1 || defined COMM2
#define COMMS
#endif

#ifdef COMM2
#define MODBUS
#endif


//#define Flashw25qxx
#define Flashat25sf321
#define SPIFFS


#define D_FileNameLength 22

void Error(char* s);

#endif