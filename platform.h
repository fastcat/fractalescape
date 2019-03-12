#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdlib.h>

void logstr(const char *str, int loglevel = 1);
void initlogstr(void * = NULL);
void termlogstr();		//should call showterminate
bool requestAbort();
void doAbort();


#endif //__PLATFORM_H__