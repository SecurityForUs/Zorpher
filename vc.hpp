/********************************************
 * Zorpher
 *
 * Vigenere Cipher:
 * http://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher
 *
 * Example:
 * Plain-text: ENCRYPTION
 * Key:        KEYKEYKEYK
 * Encryption: ORABCNDMMX
 *
 * let n = 0 (base = 0)
 * 
 * P = plain-text, K = key, C = cipher-text
 *
 * ENC() = ENCRYPT
 * DEC() = DECRYPT
 *
 * ENC(P) = (Pn + Kn) % 26 (while (n+1) != end of P)
 * DEC(C) = (Cn - Kn) % 26 (while (n+1) != end of C)
 *
 * The original cipher only used the uppercase English alphabet for keys.
 * This program uses those, and offers a chance to use others.
 * MODULO 26 = uppercase alphabet (cipher standard)
 * MODULO 52 = MODULO 26 and lowercase alphabet
 * MODULO 94 = MODULO 52 and all other characters in ASCII
 *
 * All vc_* functions (i.e.: vc_key()) are public.
 *
 * MODULO value is based on number of characters in table[].
 *
 * Please note that the cipher is dependent on the table[]...
 *
 * Developed as a proof of concept, this is meant to re-evaulate this cipher,
 * as well as propose ideals on how to combat the OTP (one-time pad) cipher scheme.
 ***********************************************/
#ifndef __KEY_H
#define __KEY_H

// This is for uint64_t
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "random.hpp"

/**
 * table[]
 * Look up table for all of the possible values in the key (used for deciphering)
 *
 * Table is base-0
 *
 * Usage:
 *
 * Return the character at position (Pc + Kc) for enciphering, or (Cc - Kc) for deciphering.
 *
 * By definition, using a static table can break the purpose of OTP, but since the key itself
 * is different upon each key-gen, it's trivial to worry about the table being an issue.
 **/
const char table[94] = {
                        // MODULO == 26
                        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                        // MODULO == 52
                        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                        // MODULO == 94
                        ' ', '!', '"', '#', '$', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':',
                        // -- 78
                        ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~'
};

class VC {
	private:
		int MODULO;
		int VC_KEY;

		string vkey;

		int tbl_lookup(char);
		int ret_range(int,int,int);
		int key();

		char encipher(char,char);
		char decipher(char,char);
	public:
		VC(int,int);
		void encrypt(const char*,const char*,string&);
		void decrypt(const char*,const char*,string&);

		string GetKey(){ return vkey; }
		const char *szGetKey(){ return vkey.c_str(); }

		void SetKey(){ key(); }
		void SetKey(string &k){ vkey = k; }
};

#endif
