#include "GlobalKey.h"

#ifdef Comm1
#define COMMS
#include "SuperLoop_Comm.c"
#endif

#ifdef COMM2
#define COMMS
#include "SuperLoop_Comm2.c"
#endif
