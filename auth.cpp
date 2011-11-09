#include "auth.hpp"

Auth::Auth(int type, string user, string pass){
	// By default, we don't allow successful authentication
	m_auth = 0;

	if(type == AUTH_SHADOW)
		m_auth = Shadow(user, pass);
}

/**
 * GetSalt()
 * Returns actual salt (everything before encrypted password) from shadow entry
 *
 * 4 $'s - opts not null, non-DES
 * 3 $'s - opts null, non-DES
 * 2 $'s - DES cipher, opts null
 **/
const char *Auth::GetSalt(const char *pwdp){
	string stmp(pwdp);
	string salt;
	salt.resize(256);

	size_t pos = stmp.find("rounds=");

	if(pos != string::npos){
		size_t pos1 = stmp.find("$", pos);
		size_t pos2 = stmp.find("$", pos1+1);

		salt = stmp.substr(0,pos2);
	} else
		salt = stmp.substr(0, 11);

	return salt.c_str();
}

/**
 * Authenticate against server's local /etc/shadow file
 *
 * user: Username of user
 * pass: Given password for user
 *
 * Returns 1 on success (valid authentication), 0 on incorrect credentials.
 **/
int Auth::Shadow(string user, string pass){
	// Holds the complete /etc/shadow structure for user
	struct spwd *spw = NULL;

	// Typical salts are only 11 characters long in /etc/shadow
	char *salt = new char[256];

	// Empty out salt buffer (memory management practice)
	memset(salt, '\0', sizeof(salt));

	// Success?  (default: no)
	int res = 0;

	// Attempt to load user information from shadow file into structure
	if((spw = getspnam(user.data())) != (struct spwd*)0){
		// I prefer doing this via C++...but GetSalt() is causing issues currently when using C++
		sprintf(salt, "%s", GetSalt(spw->sp_pwdp));

		// Check if user's provided password is correct
		if(streq(crypt(pass.data(), salt), spw->sp_pwdp))
			res = 1;
	} else{
		setspent();

		int i = 0;

		while((spw = getspent()) != (struct spwd*)0){
			i++;

			if(streq(spw->sp_namp, user.data())){
				strncat(salt, spw->sp_pwdp, 11);

				if(streq(crypt(pass.data(), salt), spw->sp_pwdp))
					res = 1;
			}
		}

		endspent();
	}

	// Free memory
	delete[] salt;

	// Make a notice in /var/log/auth.log of the user we are authenticating
	SYSLOG("Shadow authentication: %s for %s", ((res == 1) ? "SUCCESS" : "FAILURE"), user.data());

	return res;
}
