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

	Auth a;

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

	// If authentication worked, send OK msg
	if(a.Shadow(user, pass))
		conn->Send("OK");
	else
		conn->Send("FAIL");

	conn = NULL;
}

int main(int argc, char *argv[]){
	openlog("Zorpher [server]", LOG_ODELAY, LOG_AUTHPRIV);

	pthread_t thread;

	int threadret;

	try{
		ServerSocket server(5586);

		while(1){
			ServerSocket newconn;
			server.accept(newconn);

			try{
				threadret = pthread_create(&thread, NULL, ConnThread, static_cast<void*>(&newconn));
				pthread_join(thread, NULL);

				LOG(("Thread return status: %d (%s)", threadret, ((threadret == 0) ? "OK" : "Error")));
			} catch(SocketException &e){
				cout << "Exception: " << e.description() << "\n";
			}
		}
	} catch(SocketException &e){
		std::cout << "Exception caught: " << e.description() << "\nExiting now.\n";
	}

	return 0;
}
