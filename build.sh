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

#	SHADOW=$(find /usr/lib* -name libshadow.*)
#
#	if [ -z "$SHADOW" ]; then
#		echo "libshadow must be installed."
#		exit 1
#	fi

	PTHREAD=$(find /usr/lib* -name libpthread.*)

	if [ -z "$PTHREAD" ]; then
		echo "libpthread must be installed."
		exit 1
	fi

#	if [ -n "$CRYPT" ] && [ -n "$SHADOW" ] && [ -n "$PTHREAD" ]; then
	if [ -n "$CRYPT" ] && [ -n "$PTHREAD" ]; then
		g++ -O2 -o zorpher server.cpp Socket.cpp ServerSocket.cpp auth.cpp random.cpp vc.cpp -lcrypt -lpthread
	fi
elif [ "$TYPE" == "c" ]; then
	echo "Compiling PAM module for Zorpher."

	[[ -e "zorpher_client.o" ]] && rm -rf zorpher.client.o

	g++ -fPIC -shared -lpam client.cpp Socket.cpp ClientSocket.cpp random.cpp vc.cpp -o pam_zorpher.so
	sudo cp pam_zorpher.so /lib/security/

#	g++ -fPIC -c client.cpp Socket.cpp ClientSocket.cpp random.cpp vc.cpp
#	sudo ld -x --shared -lpam -o /lib/security/pam_zorpher.so client.o

#	g++ -o zorpher_client.o client.cpp Socket.cpp ClientSocket.cpp random.cpp vc.cpp
#	ld -G -o /lib/security/pam_zorpher.so zorpher_client.o -lpam

	rm -rf zorpher.o
fi

#echo "Compiling program..."
#g++ -g -o client client.cpp Socket.cpp ClientSocket.cpp random.cpp vc.cpp
#g++ -g -o server server.cpp Socket.cpp ServerSocket.cpp auth.cpp random.cpp vc.cpp -lcrypt -lshadow -lpthread

#if [ -e "server" ]; then
#	echo "!! Executing server !!"

#	./server
#fi
