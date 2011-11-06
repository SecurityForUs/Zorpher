#include "auth.hpp"

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
	char *salt = new char[11];
	memset(salt, '\0', sizeof(salt));

	// Success?  (default: no)
	int res = 0;

	// Attempt to load user information from shadow file into structure
	if((spw = getspnam(user.data())) != (struct spwd*)0){
		// Copy first 11 characters of user's password field ($salt$password)
		strncat(salt, spw->sp_pwdp, 11);

		LOG(("getspnam(): Checking password %s against user %s (salt: %s)", pass.data(), user.data(), salt));

		// Check if user's provided password is correct
		if(streq(crypt(pass.data(), salt), spw->sp_pwdp))
			res = 1;
	} else{
		LOG(("getspanam() failed, trying getspent()"));

		setspent();

		int i = 0;

		while((spw = getspent()) != (struct spwd*)0){
			i++;

			LOG(("spw->sp_namp = %s", spw->sp_namp));

			if(streq(spw->sp_namp, user.data())){
				strncat(salt, spw->sp_pwdp, 11);

				LOG(("getspent(): Checking password %s against user %s (salt: %s)", pass.data(), user.data(), salt));

				if(streq(crypt(pass.data(), salt), spw->sp_pwdp))
					res = 1;
			}
		}

		LOG(("Ran through %d entries in /etc/shadow.", i));

		endspent();
	}

	// Free memory
	delete[] salt;

	// Make a notice in /var/log/auth.log of the user we are authenticating
	SYSLOG("Attempting authentication [SHADOW] for user [%s} returned %s", user.data(), ((res == 1) ? "SUCCESS" : "FAILURE"));

	return res;
}
