#ifndef __NETWORK_H
#define __NETWORK_H

#include <string.h>
#include <enet/enet.h>

#include "defs.h"

void ReadData(char buff[], ENetEvent e){
	sprintf(buff, "%s", (char*)e.packet->data);
}

ENetAddress GetAddr(char *host, int port){
	ENetAddress addr;

	enet_address_set_host(&addr, host);
	addr.port = port;

	return addr;
}

/**
 * NetInit()
 * type - 1 = server, 0 = client
 * host - IP or hostname to connect to/create server for
 * port - The port to establish
 *
 * Initializes ENet and creates a socket/host
 *
 * Usage: NetInit(0, "localhost", 1337);
 *
 * Returns: ENetHost on success, NULL on failure
 **/
ENetHost *NetInit(int type, char *h, int port){
	ENetHost *host;

	if(!type)
		host = enet_host_create(NULL, 1, 2, 5760/8, 1440/8);
	else{
		ENetAddress addr = GetAddr(h, port);

		host = enet_host_create(&addr, 100, 2, 0, 0);
	}

	return host;
}

ENetPeer *NetConnect(ENetHost *host, char *h, int port){
	ENetAddress addr = GetAddr(h, port);

	ENetPeer *peer;

	peer = enet_host_connect(host, &addr, 2, 0);

	return peer;
}

/**
 * CreatePacket()
 * data - Payload for the packet
 *
 * Creates a packet, without transmitting it over the wire.
 *
 * Usage: CreatePacket("make me a packet!");
 *
 * Returns: packet on success, NULL on failure.
 **/
ENetPacket *CreatePacket(const char *data){
	LOG(("CreatePacket(%s)", data));
	return enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
}

/**
 * CatPacket()
 * pkt - Packet data to manipulate
 * data - The data to append to pkt->data
 *
 * Modifies pkt->data, appending data to the end of the data already stored.
 *
 * Usage: CatPacket(&pkt, "append");
 *
 * Returns: N/A
 **/
void CatPacket(ENetPacket *pkt, const char *data){
	size_t pktd = strlen((char*)pkt->data);
	size_t len = pktd + strlen(data) + 1;

	enet_packet_resize(pkt, len);
	strcpy((char*)&pkt->data[pktd], data);
}

/**
 * SendPacket()
 * peer - The peer (server/client) to send data to
 * data - The data to send to the peer
 *
 * Creates a packet of data, and then sends data to peer.
 *
 * Usage: SendPacket(&peer, "transmitted over the wire.");
 *
 * Returns: 1 on success, 0 on failure.
 **/
int SendPacket(ENetPeer *peer, const char *data){
	LOG(("Data: %s", data));

	ENetPacket *packet = CreatePacket(data);

	enet_peer_send(peer, 0, packet);

	enet_packet_destroy(packet);
}

#endif
