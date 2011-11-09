#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Used for multithreading
#include <pthread.h>

#include "defs.h"

// For networking stuff
#include "ServerSocket.h"
#include "SocketException.h"

// Randomization (for VC keygen)
#include "random.hpp"

// Viegnere Cipher
#include "vc.hpp"

// For authentication
#include "auth.hpp"

/**
 * ConnThread()
 *
 * Thread generated for each connection made.
 **/
void *ConnThread(void *data){
	string buff;
	string encrypt;
	string decrypt;
	string user;
	string pass;

	ServerSocket *conn = static_cast<ServerSocket*>(data);

	// Create a Viegnere Cipher key
	VC vc(10,94);
	vc.SetKey();

	// Get username
	conn->Send(vc.GetKey());
	conn->Recv(buff);

	// Decrypt username to readable format
	vc.decrypt(buff.c_str(), vc.szGetKey(), user);

	// Get password
	conn->Recv(buff);

	// Do the same
	vc.decrypt(buff.c_str(), vc.szGetKey(), pass);

	// Perform authentication method
	Auth a(AUTH_SHADOW, user, pass);

	// If authentication worked, send OK msg
	if(a.GetAuth())
		conn->Send("OK");
	else
		conn->Send("FAIL");

	conn = NULL;
}

int main(int argc, const char *argv[]){
	openlog("Zorpher [server]", LOG_ODELAY, LOG_AUTHPRIV);

	struct options opts;

	parse_options(argc, argv, &opts);

	pthread_t thread;

	int threadret;

	try{
		LOG(("Server: %s:%d", opts.server, opts.port));

		ServerSocket server(opts.server, opts.port);

		SYSLOG("Running on %s:%d", opts.server, opts.port);

		while(1){
			ServerSocket newconn;
			server.accept(newconn);

			try{
				// On client connection, try to create a new thread (per connection)
				threadret = pthread_create(&thread, NULL, ConnThread, static_cast<void*>(&newconn));
				pthread_join(thread, NULL);
			} catch(SocketException &e){
				cout << "Socket exception: " << e.description() << "\nExiting.\n";
			}
		}
	} catch(SocketException &e){
		std::cout << "Exception caught: " << e.description() << "\nExiting now.\n";
	}

	return 0;
}
