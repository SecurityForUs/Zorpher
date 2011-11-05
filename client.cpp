#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ClientSocket.h"
#include "SocketException.h"

#include "vc.hpp"

#include "defs.h"

int main(int argc, char *argv[]){
	string buffer;
	string user;
	string pass;
	string enc;
	string dec;

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
				LOG(("Successful login."));
			else
				LOG(("Incorrect credentials."));
		} catch(SocketException &e){
			cout << "Exception: " << e.description() << "\n";
		}

		cout << "Response recieved: " << buffer << "\n";
	} catch(SocketException &e){
		cout << "Exception caught: " << e.description() << "\n";
	}

	return 0;
}
