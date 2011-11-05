#include <string.h>
#include <fstream>
#include "random.hpp"

/**
 * Rand()
 * bytes: How many bytes to read from /dev/urandom
 * buff: Where to store the valid characters read
 *
 * Reads [bytes] bytes of data from /dev/urandom, checks to make sure they are human-readable, and if so stores the character into buff.
 **/
ZRandom::ZRandom(string &buff){
	FILE *fp;

	size_t len = 0;
	size_t bytes = buff.capacity();

	char c = '\0';

	if((fp = fopen("/dev/urandom", "rb")) != NULL){
		while(len < bytes){
			c = fgetc(fp);

			if((c >= ' ') && (c <= '~')){
				buff[len] = c;

				len++;
			}
		}
	}

	fclose(fp);
}
