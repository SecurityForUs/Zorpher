#ifndef __DEFS_H
#define __DEFS_H

// Non-specific stuff
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
#define DEBUG 0

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

// Strings are equal
#define streq(a,b) !strcmp(a,b)

// n numbers of characters are equal
#define strneq(a, b, n) !strncmp(a,b,n)

#ifndef INADDR_ANY
	#define INADDR_ANY ((unsigned long int) 0x00000000)
#endif

// Function to use syslog
static void SYSLOG(const char *f, ...){
	char buffer[256];

	va_list args;
	va_start(args, f);
	vsprintf(buffer, f, args);
	va_end(args);

	#if DEBUG == 1
		LOG(buffer);
	#endif

	syslog(LOG_INFO, "%s", buffer);
}

// Structure for various options allowed
struct options {
        char *server;
        int port;
	int auth_type;
};

/**
 * Parses arguments passed to module.
 *
 * Returns nothing.
 **/
static void parse_options(int argc, const char **argv, struct options *opts){
        // Defaults
        opts->server = INADDR_ANY;
        opts->port = 5586;

	char *tmp = new char[20];

        for(int i = 0; i < argc; i++){
                // What's the server to connect to?
                if(strneq(argv[i], "server=", 7))
                        opts->server = (char*)argv[i] + 7;

                // A different port?
                else if(strneq(argv[i], "port=", 5))
                        opts->port = atoi(argv[i] + 5);

		else if(strneq(argv[i], "auth=", 5)){
			tmp = (char*)argv[i] + 5;

			if(streq(tmp, "mongo"))
				opts->auth_type = 2;
			else
				opts->auth_type = 1;
		}

        }

	delete[] tmp;
}

using namespace std;

#endif
