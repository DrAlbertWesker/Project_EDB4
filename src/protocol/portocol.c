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
#include "../input/input_service.h"

static uint16_t gTransactionId = 0;

SendPacket_t* createPlayerRegistrationPacket(uint16_t transactionId, char* playername) {

	gTransactionId = transactionId;
	uint16_t packetLength = strlen(playername) + HEADER_LENGTH +3;
	SendPacket_t* pPlayerReg = malloc(sizeof(SendPacket_t));
	if (pPlayerReg == NULL) {
		return NULL;
	}
	uint8_t* pBuffer = malloc(sizeof(packetLength));
	if (pBuffer == NULL) {
		return NULL;
	}
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

SendPacket_t* createPlayerControlPacket(bool up, bool right, bool down, bool left) {
	uint8_t HEADERSIZE = 7;
	uint8_t PL = 1;
	uint8_t* pCtrlPacket = malloc(HEADERSIZE + PL);
	if (pCtrlPacket == NULL) {
		return NULL;
	}
	SendPacket_t* pPacket = malloc(sizeof(SendPacket_t));
	pCtrlPacket[0] = 1;
	write_msblsb(&pCtrlPacket[1], 1);
	write_msblsb(&pCtrlPacket[3], PLAYER_CONTROL);
	write_msblsb(&pCtrlPacket[5], gTransactionId);
	uint8_t ctrlCmd = 0;
	if (up == true) {
		ctrlCmd = (ctrlCmd | INPUT_KEY_MASK_KEY_UP);
	}
	if (right == true) {
		ctrlCmd = (ctrlCmd | INPUT_KEY_MASK_KEY_RIGHT);
	}
	if (down == true) {
		ctrlCmd = (ctrlCmd | INPUT_KEY_MASK_KEY_DOWN);
	}
	if (left == true) {
		ctrlCmd = (ctrlCmd | INPUT_KEY_MASK_KEY_LEFT);
	}
	pCtrlPacket[7] = ctrlCmd;
	pPacket->pBuf = pCtrlPacket;
	pPacket->size = (HEADERSIZE + PL);
	return pPacket;

}
