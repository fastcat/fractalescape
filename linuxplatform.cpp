//#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "platform.h"
#include "FECode.h"

//globals for logstr
FILE *LogStrFile = NULL;
char *LogStrBuffer = NULL;
unsigned long LogStrBufPos = 0;
const unsigned long LogStrBufLen = 16384;
bool AbortFlag = false;

void install_sigint_handler();

// we replace kbhit detection with nice ctrl-c detection
// This is our signal handler
void sigint_handler(int sig)
{
	doAbort();
	// As a safety catch, we re-install the signal handler
	install_sigint_handler();
};

// This function installs the sigint_handler
void install_sigint_handler()
{
	signal(SIGINT, sigint_handler);
};


void logstr(const char *str, int loglevel)
{
//	TRACE(str);
	if (LogToScreen >= loglevel)
	{
		printf(str);
		fflush(stdout);
	};
	if (LogToFile >= loglevel)
	{
		bufwrite(str, strlen(str), LogStrFile, LogStrBuffer, LogStrBufPos, LogStrBufLen);
	}
};

void initlogstr(void*)
{
	srand((unsigned)time(NULL));		//reseed the random number generator
	LogStrFile = fopen("FractalEscapeLog.txt", "a");
	LogStrBuffer = new char[LogStrBufLen];
	LogStrBufPos = 0;
	MainStart = time(NULL);		//the time the program started
	logstr(VTag);
	logstr(" Starting Up On ");
	logstr(ctime(&MainStart));		//ctime includes a newline
	// Install the ctrl-c handler
	install_sigint_handler();
};

void termlogstr()
{
	showterminate();
	if (LogStrBufPos > 0)
		fwrite(LogStrBuffer, LogStrBufPos, 1, LogStrFile);
	if (LogStrFile != NULL)
		fclose(LogStrFile);
	// uninstall the sigint handler
	signal(SIGINT, SIG_DFL);
	
#ifdef _DEBUG
	while (kbhit())
		getch();
	while (!kbhit())
		;
#endif
};

bool requestAbort()
{
	return AbortFlag;
};

void doAbort()
{
	AbortFlag = true;
};


