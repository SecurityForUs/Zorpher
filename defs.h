#ifndef __DEFS_H
#define __DEFS_H

// Non-specific stuff
#include <stdio.h>

// C++-specific stuff
#include <iostream>
#include <vector>

// Time stuff
#include <time.h>
#include <sys/time.h>

// For syslogging
#include <syslog.h>
#include <stdarg.h>

// Verbose display
//	Set DEBUG = 0 for production release
#define DEBUG 1

//	Verbose output
#if DEBUG == 1
	#define LOG(x) do {									\
				struct timeval tmp;						\
				struct timezone tz;						\
				gettimeofday(&tmp, &tz);					\
				printf("%ld.%06ld ", tmp.tv_sec, tmp.tv_usec);			\
				printf("[%s:%d] (%s)\t ", __FILE__, __LINE__, __FUNCTION__);	\
				printf x;							\
				printf("\n");							\
	} while(0)
#else
	#define LOG(x) do{			\
				printf x;	\
				printf("\n");	\
	} while(0)
#endif

// Default server IP/host
#define DEFHOST "192.168.1.42"

// Default port of connection
#define DEFPORT 5866

// Default buffer for data transmission
#define DEFBUFF 4098

// Strings are equal
#define streq(a,b) !strcmp(a,b)

// n numbers of characters are equal
#define strneq(a, b, n) !strncmp(a,b,n)

// Function to use syslog
static void SYSLOG(char *f, ...){
	char buffer[256];

	va_list args;
	va_start(args, f);
	vsprintf(buffer, f, args);
	va_end(args);

	syslog(LOG_INFO, buffer);
}

using namespace std;

#endif
