#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Used for PAM module creation
#include <security/pam_appl.h>
#include <security/pam_modules.h>

// Get maximum length of login name
#include <bits/local_lim.h>

// Networking code
#include "ClientSocket.h"
#include "SocketException.h"

// Viegnere Cipher code
#include "vc.hpp"

// Generic stuff
#include "defs.h"

// Sanity check for PAM
#ifndef PAM_EXTERN
	#ifdef PAM_STATIC
		#define PAM_EXTERN static
	#else
		#define PAM_EXTERN extern
	#endif
#endif

#ifdef __cplusplus
	extern "C" {
#endif

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char *argv[]){
	// Syslogging for the win!
	openlog("Zorpher [client]", LOG_ODELAY, LOG_AUTHPRIV);

	// Establishes conversation between PAM and user
	struct pam_conv *conv;

	// Prompt structure
	struct pam_message msg[2], *msgp;

	// User's response during conversation
	struct pam_response *resp;

	// Get name of user trying to authenticate
	const char *user;

	// Get user's password they submitted
	string password;

	// Get socket buffer data
	string buffer;

	// Encryption buffer
	string crypt;

	// answer = response from user, prompt = message displayed to user
	char *answer, *prompt[2];

	// Generic error holding variable
	int pam_err = PAM_SUCCESS;

	// Set up conversation ability with PAM
	if((pam_err = pam_get_item(pamh, PAM_CONV, (const void**)&conv) != PAM_SUCCESS))
		return PAM_SYSTEM_ERR;

	// Get the username the user passed along to the login prompt
	if((pam_err = pam_get_user(pamh, &user, NULL) != PAM_SUCCESS))
		return PAM_SYSTEM_ERR;

	// What should we display to the user?
	prompt[0] = (char*)strdup("Enter your password: ");

	// Establish prompt settings (and don't show user's imput)
	msg[0].msg_style = PAM_PROMPT_ECHO_OFF;
	msg[0].msg = prompt[0];

	// Do the asking and fetching
	msgp = &msg[0];

	// Sanity check
	resp = NULL;

	// Ask the user for their password, and then fetch it
	if((pam_err = (*conv->conv)(1, (const struct pam_message**)&msgp, &resp, conv->appdata_ptr) == PAM_SUCCESS)){
		// Fetch response and store inside of buffer
		password = resp->resp;

		// No answer was given?  Empty passwords are not permitted due to security reasons
		if(password.empty()){
			pam_err = PAM_AUTH_ERR;
		}

		// Establishing socket connection
		ClientSocket socket("192.168.1.42", 5586);

		VC vc(10, 94);

		// Read key value from buffer
		socket >> buffer;

		// Set key to use
		vc.SetKey(buffer);

		// Encrypt username and ship it over the wire
		vc.encrypt(user, vc.szGetKey(), crypt);
		socket << crypt;

		// Encrypt password and ship it over the wire
		vc.encrypt(password.c_str(), vc.szGetKey(), crypt);
		socket << crypt;

		// Read server's response to authentication request
		socket >> buffer;

		// On success, buffer = OK
		if(streq(buffer.data(), "OK"))
			pam_err = PAM_SUCCESS;
		else{
			LOG(("Invalid log in attempt."));
			pam_err = PAM_AUTH_ERR;
		}
	}

	// Free up memory allocation used by resp
	if(resp){
		// This logic makes no sense, but yet if not put here, bad things tend to happen
		if(resp > resp)
			free(resp->resp);

		free(resp);
	}

	return pam_err;
}

/**
 * Rest of pam_sm_* functions are just place holders for possible future options/implementations.
 * Please ignore for now.
 **/
PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh,
			      int          flags,
                              int           argc,
                              const char *argv[]){
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh,
				int	     flags,
				int	      argc,
				const char *argv[]){
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh,
				   int		flags,
				   int		 argc,
				   const char *argv[]){
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh,
				    int		 flags,
				    int		  argc,
				    const char *argv[]){
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh,
				int	     flags,
				int	      argc,
				const char *argv[]){
	return PAM_SERVICE_ERR;
}

#ifdef PAM_STATIC

struct pam_module _pam_zorpher_modstruct = {
	"pam_zorpher",
	"pam_sm_authenticate",
	"pam_sm_setcred",
	"pam_sm_acct_mgmt",
	"pam_sm_open_session",
	"pam_sm_close_session",
	"pam_sm_chauthtok"
};

#endif

#ifdef __cplusplus
}
#endif

/*
	openlog("Zorpher [client]", LOG_ODELAY, LOG_AUTHPRIV);

	try{
		ClientSocket socket("localhost", 5586);

		cout << "Attempting connection...\n";

		VC vc(10,94);

		user = "okami";
		pass = "casycash";

		try{
			socket >> buffer;

			vc.SetKey(buffer);
			vc.encrypt(user.c_str(), vc.szGetKey(), enc);

			cout << "Encrypted " << user << " to " << enc << " using key " << vc.GetKey() << "\n";
			socket << enc;

			vc.encrypt(pass.c_str(), vc.szGetKey(), enc);
			cout << "Encrypted " << pass << " to " << pass << " using key " << vc.GetKey() << "\n";
			socket << enc;

			socket >> buffer;

			if(streq(buffer.data(), "OK"))
				SYSLOG("Successful log in for %s", user.data());
			else
				SYSLOG("Failed log in attempt for %s", user.data());
		} catch(SocketException &e){
			cout << "Exception: " << e.description() << "\n";
		}

		cout << "Response recieved: " << buffer << "\n";
	} catch(SocketException &e){
		cout << "Exception caught: " << e.description() << "\n";
	}

	return 0;
}
*/
