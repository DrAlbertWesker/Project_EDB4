/*
 * portocol.c
 *
 *  Created on: 22.06.2020
 *      Author: nicom
 */
#include "protocol.h"
#include "../communicator/communicator.h"
#include <stdlib.h>
#include <string.h>

SendPacket_t* createPlayerRegistrationPacket(uint16_t transactionId, char* playername) {

	uint16_t packetLength = strlen(playername) + HEADER_LENGTH +3;
	SendPacket_t* pPlayerReg = malloc(sizeof(SendPacket_t));
	uint8_t* pBuffer = malloc(sizeof(packetLength));
	PacketHeader_t* pHeader = (PacketHeader_t*) pBuffer;
	pBuffer[0] = 0;
	pHeader->Request = 1;
	pBuffer[0] = 1;
	write_msblsb(&pBuffer[1], (packetLength - HEADER_LENGTH));
	write_msblsb(&pBuffer[3], REGISTER_PLAYER);
	write_msblsb(&pBuffer[5], transactionId);
	pBuffer[7] = PlAYER_NAME;
	write_msblsb(&pBuffer[8], strlen(playername));
	memcpy(&pBuffer[10], playername, strlen(playername));
	pPlayerReg->pBuf = &pBuffer[0];
	pPlayerReg->size = packetLength;
	return pPlayerReg;

}

