#!/bin/bash

rm -rvf server client

echo "Compiling program..."
g++ -g -o client client.cpp Socket.cpp ClientSocket.cpp random.cpp vc.cpp
g++ -g -o server server.cpp Socket.cpp ServerSocket.cpp auth.cpp random.cpp vc.cpp -lcrypt -lshadow -lpthread

if [ -e "server" ]; then
	echo "!! Executing server !!"

	./server
fi
