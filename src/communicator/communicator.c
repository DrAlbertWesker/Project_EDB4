/*
 * communicator.c
 *
 *  Created on: 07.05.2020
 *      Author: nicom
 */
#include "communicator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SERVER_IP_ADDRESS					("195.34.89.241")
#define DEFAULT_SERVER_PORT							(7)

static void cbNetworkReceive(uint8_t* pBuffer, uint32_t len);

int communicator_connect(Server_e server) {

	if (!network_init(cbNetworkReceive)) {
		return -1;
	}

	switch (server) {
	case TEST_SERVER:
	case MAIN_SERVER:
		return -3;

	case ECHO_SERVER:
		if (!network_connect(DEFAULT_SERVER_IP_ADDRESS, DEFAULT_SERVER_PORT)) {
			return -2;
		}
		break;

	default:
		return -3;
	}

	return 0;
}

setPacketVersion(uint8_t* pPacket, uint32_t len) {
	pPacket[0] &= ~(0b11100000);
	pPacket[1] = 0;
}

int communicator_createSesson() {

	header_t* newSession = malloc(sizeof(header_t));
	if (newSession == NULL) {
		return -404;
	}

	newSession->version = 0;
	newSession->sessionRequest = 1;
	//...
	network_send(newSession, 5);		// Client sendet challenge request

	// Server sendet challenge
	// Client beantwortet challenge


	// Server kontrolliert und akzeptiert wenn korrekt

	return 0;
}

void cbNetworkReceive(uint8_t* pBuffer, uint32_t len) {
	printf("Packet received with len=%d.\r\n", len);
}
