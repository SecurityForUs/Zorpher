#ifndef __AUTH_H
#define __AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include <shadow.h>
#include <crypt.h>
#include <pwd.h>

#include "defs.h"

#define AUTH_SHADOW	1
#define AUTH_PAMMONGODB	2

class Auth {
	private:
		const char*GetSalt(const char*);

		// Authenticate user via /etc/shadow file
		int Shadow(string,string);

		// 0 = fail, 1 = succes
		int m_auth;

	public:
		Auth(int,string,string);
		int GetAuth(){ return m_auth; }
};

#endif
