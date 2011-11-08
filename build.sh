#!/bin/bash

#rm -rvf server client

TYPE="$1"

if [ -z "$TYPE" ]; then
	echo "Usage: $0 <s|c>"
	echo -e "\ts - Compile server module\n\tc - Compile client module\n"

	exit 1
fi

if [ "$TYPE" == "s" ]; then
	echo "Compiling server module for Zorpher."

	CRYPT=$(find /usr/lib* -name libcrypt.*)

	if [ -z "$CRYPT" ]; then
		echo "libcrypt must be installed."
		exit 1
	fi

	PTHREAD=$(find /usr/lib* -name libpthread.*)

	if [ -z "$PTHREAD" ]; then
		echo "libpthread must be installed."
		exit 1
	fi

	if [ -n "$CRYPT" ] && [ -n "$PTHREAD" ]; then
		g++ -O2 -o zorpher server.cpp Socket.cpp ServerSocket.cpp auth.cpp random.cpp vc.cpp -lcrypt -lpthread
		rm -rf *.o
	fi
elif [ "$TYPE" == "c" ]; then
	echo "Compiling PAM module for Zorpher."

	[[ -e "zorpher_client.o" ]] && rm -rf zorpher.client.o

	g++ -fPIC -shared -lpam client.cpp Socket.cpp ClientSocket.cpp random.cpp vc.cpp -o pam_zorpher.so
	sudo cp pam_zorpher.so /lib/security/

	rm -rf zorpher.o
fi

exit 0
