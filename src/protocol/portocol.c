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
static uint8_t gHeadersize = 7;

SendPacket_t* createPlayerRegistrationPacket(uint16_t transactionId, char* playername, uint8_t* pAvatar, PlayerColor_t* color) {
	gTransactionId = transactionId;
	uint8_t commandRegisterSize = 3;
	uint16_t nameDataLen = strlen(playername);
	uint16_t imageDataLen = sizeof(pAvatar);
	uint16_t colorDataLen = sizeof(PlayerColor_t);
	uint32_t packetLength = HEADER_LENGTH + (3 * commandRegisterSize) + nameDataLen + imageDataLen + colorDataLen;
	SendPacket_t* pPlayerReg = malloc(sizeof(SendPacket_t));
	if (pPlayerReg == NULL) {
		return NULL;
	}
	uint8_t* pBuffer = malloc(packetLength);
	if (pBuffer == NULL) {
		return NULL;
	}
	pBuffer[0] = 1;
	write_msblsb(&pBuffer[1], (packetLength - HEADER_LENGTH));
	write_msblsb(&pBuffer[3], REGISTER_PLAYER);
	write_msblsb(&pBuffer[5], transactionId);
	pBuffer[7] = PlAYER_NAME;
	write_msblsb(&pBuffer[8], nameDataLen);
	memcpy(&pBuffer[10], playername, nameDataLen);
	pBuffer[10 + nameDataLen] = PLAYER_IMAGE;
	write_msblsb(&pBuffer[11 + nameDataLen], imageDataLen);
	memcpy(&pBuffer[13 + nameDataLen], pAvatar, imageDataLen);
	pBuffer[13 + nameDataLen + imageDataLen] = PLAYER_COLOR;
	write_msblsb(&pBuffer[14 + nameDataLen + imageDataLen], colorDataLen);
	pBuffer[16 + nameDataLen + imageDataLen] = color->red;
	pBuffer[17 + nameDataLen + imageDataLen] = color->green;
	pBuffer[18 + nameDataLen + imageDataLen] = color->blue;
	pPlayerReg->pBuf = &pBuffer[0];
	pPlayerReg->size = packetLength;
	return pPlayerReg;
}

SendPacket_t* createPlayerControlPacket(bool up, bool right, bool down, bool left) {
	uint8_t PL = 1;
	uint8_t* pCtrlPacket = malloc(gHeadersize + PL);
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
	pPacket->size = (gHeadersize + PL);
	return pPacket;
}

SendPacket_t* createPlayerChatPacket(char* message) {
	uint16_t stringLength = strlen(message);
	uint16_t packetLength = stringLength + HEADER_LENGTH;
	SendPacket_t* pMessage = malloc(sizeof(SendPacket_t));
	if (pMessage == NULL) {
		return NULL;
	}
	uint8_t* pBuffer = malloc(sizeof(packetLength));
	if (pBuffer == NULL) {
		return NULL;
	}
	pBuffer[0] = 1;
	write_msblsb(&pBuffer[1], (packetLength - HEADER_LENGTH));
	write_msblsb(&pBuffer[3], CHAT_MSG);
	write_msblsb(&pBuffer[5], gTransactionId);
	memcpy (&pBuffer[7], message, (stringLength));
	pMessage->pBuf = &pBuffer[0];
	pMessage->size = packetLength;
	return pMessage;
}


SendPacket_t* createPlayerDropFoodPacket(void) {
	uint8_t PL = 1;
	uint8_t* pDropFoodPacket = malloc(gHeadersize + PL);
	if (pDropFoodPacket == NULL) {
		return NULL;
	}
	SendPacket_t* pPacket = malloc(sizeof(SendPacket_t));
	pDropFoodPacket[0] = 1;
	write_msblsb(&pDropFoodPacket[1], 1);
	write_msblsb(&pDropFoodPacket[3], DROP_FOOD);
	write_msblsb(&pDropFoodPacket[5], gTransactionId);
	pDropFoodPacket[7] = 0;
	pPacket->pBuf = pDropFoodPacket;
	pPacket->size = (gHeadersize + PL);
	return pPacket;

}

