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

class Auth {
	public:
		int Shadow(string,string);
};

#endif
