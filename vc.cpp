#include "vc.hpp"

/**
 * VC()
 * keysize: How big of a key do we want (if smaller than message, key is repeated from beginning to end)
 * range: Alphanumeric range we want to use for table (see notes for more details)
 *
 * Initializes the VC class.
 **/
VC::VC(int keysize, int range){
	/**
	 * See mod() comments for details on this.
	 *
	 * TBL SIZE     | INCLUDES                      | STATUS
	 * -------------------------------------------------------
	 * 26           | A-Z                           | WORKS
	 * 52           | A-Z, a-z                      | WORKS
	 * 94           | A-Z, a-z, rest of ASCII table | WORKS
 	 *
	 * NOTES:
	 * - 52 is harder to code as in ASCII scan code, it has two ranges, instead of just one.
	 * It works better now than it did originally though.
	 **/
	MODULO = range;
	VC_KEY = keysize;
	vkey.resize(VC_KEY);
}

/**
 * tbl_lookup()
 * i:   Character to look up in table[] [in]
 *
 * Routine for looking up character 'i' in table[].  Returns index it is found in.
 **/
int VC::tbl_lookup(char i){
        // Since 'i' is already used, use the other generic j (base 0)
        int j = 0;

        while(j < MODULO){
                // table[j] will be the same as the character given
                if(table[j] == i)
                        break;

                // No other match, so increment j by 1
                j++;
        }

        // Since 'i' is assumed to be in the table no matter what, return the position
        return j;
}

/**
 * encihper()
 * p:   Position of plain-text character (assumes it is a char) [in]
 * k:   Position of key character (assumes it is a char)        [in]
 *
 * Returns values of table[p+k] for encryption.
 **/
char VC::encipher(char p, char k){
        // Index counters for both plain-text and key characters
        int pn = 0, kn = 0;

        pn = tbl_lookup(p);
        kn = tbl_lookup(k);

        int i = pn + kn;

        // Since table is base-0, we need to make sure we're still in the bounds of MODULO
        while(i >= MODULO){
                i -= MODULO;
        }

        return table[i];
}

/**
 * decipher()
 * c:   Cipher character        [in]
 * k:   Key character           [in]
 *
 * Decrypts a cipher back to plain text.
 *
 * Same thing as encipher() basically, except for it subtracts pn and kn.
 **/
char VC::decipher(char c, char k){
        // Index counters for both cipher and key characters
        int cn = 0, kn = 0;

        cn = tbl_lookup(c);
        kn = tbl_lookup(k);

        /**
         * table[index] = cipher_index - key_index
         *
         * Unlike in encipher(), due to math theorems, substitution MUST be this way.
         **/
        int i = cn - kn;

        if(i < 0)
                i += MODULO;

        return table[i];
}

/**
 * ret_range()
 * min: Minimum number val can be       [in]
 * max: Maximum number val can be       [in]
 * val: The number to evaluate          [in]
 *
 * Returns a number between min & max, based on the value (val).
 *
 * In future, hope to turn val into in/out, instead of just in.
 **/
int VC::ret_range(int min, int max, int val){
        while((val < min) || (val > max)){
                if(val < min)
                        val += min;
                else if(val > max)
                        val -= max;
        }

        return val;
}

/**
 * key()
 * Reads /dev/urandom and generates an ASCII key from the data.
 *
 * Returns number of bytes read if successful, 0 if failed.
 *
 * NOTE: return of key() should always be the same as bytes_read!!!
 **/
int VC::key(){
	LOG(("Generating a key of size %d with a total of %d possible characters.", VC_KEY, MODULO));

	int bytes_read = VC_KEY;

        int res, tmp, min, max = 0;

        // Allocate a big enough buffer to hold x bytes of data
	string buff;
	buff.resize(bytes_read);

        ZRandom ZR(buff);

        if(MODULO == 26){
                min = 65;
                max = 90;
        } else if(MODULO == 52){
                min = 65;
                max = 122;
        } else if(MODULO == 94){
                min = 32;
                max = 126;
        }

        tmp = 0;

	while(tmp < bytes_read){
		res = ret_range(min, max, (int)buff[tmp]);

		vkey[tmp] = res;

		tmp++;
	}

	LOG(("-> VC::key() = %s (%d)", vkey.data(), tmp));

	buff.clear();

        return tmp;
}

/**
 * encrypt()
 * p:           Plain-text to encrypt           [in]
 * k:           Key to use for encryption       [in]
 * buff:        Buffer to store encrypted data  [out]
 *
 * Encrypts given data using the key, and stores it in buff.
 *
 **/
void VC::encrypt(const char p[], const char k[], string &buff){
	buff.clear();

        // p & k will both be the same length
        int len = strlen(p);
        int keylen = strlen(k);

        // Current position inside of p & k
        int i = 0, j = 0;

        while(i < len){
                if(j == keylen)
                        j = 0;

                buff += encipher(p[i], k[j]);

                i++;
		j++;
        }
}

/**
 * decrypt()
 * c:           Cipherext			[in]
 * k:           Key used to encrypt text        [in]
 * buff:        Buffer to store decrypted data  [out]
 *
 * Decrypts cipher (using key), and stores it into buff.
 **/
void VC::decrypt(const char c[], const char k[], string &buff){
	buff.clear();

        // c & k will be the same length
        int len = strlen(c);
        int keylen = strlen(k);

        // Current position in c & k
        int i = 0, j = 0;

        // While not at the end of the cipher text
        while(i < len){
                if(j == keylen)
                        j = 0;

                buff += decipher(c[i], k[j]);

                i++;
		j++;
        }
}

