/*
 * communicator.c
 *
 *  Created on: 07.05.2020
 *      Author: nicom
 */
#include <windows.h>
#include "communicator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SERVER_IP_ADDRESS					("195.34.89.241")
#define GAME_SERVER_IP_ADDRESS						("52.57.105.0")
#define DEFAULT_SERVER_PORT							(7)
#define GAME_SERVER_PORT							(44444)
#define SERVER_CHALLENGE							0x04
#define SESSION_ESTABLISHED_						0x10
#define SESSION_HEADER_LENGTH						9


static void cbNetworkReceive(uint8_t* pBuffer, uint32_t len);
static uint16_t gSessionId;
uint16_t gSequenceNumber_tx;
uint16_t gSequenceNumber_rx;
uint16_t gChallengeResponse;

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

	case GAME_SERVER:
		if (!network_connect(GAME_SERVER_IP_ADDRESS	, GAME_SERVER_PORT)) {
			return -4;
		}
		break;

	default:
		return -3;
	}

	return 0;
}

int communicator_createSesson() {
	SendPacket_t* pCreateSession = sessionCreatePacket(0, SESSION_REQUEST, 0, 0, 0, 0);
	network_send(pCreateSession->pBuf, pCreateSession->size);
	sessionDestroyPacket(pCreateSession);
	while (gSessionId == 0) {
		Sleep(10);
	}
	return 0;
}

SendPacket_t* communicator_challengeRespond(uint32_t nonce) {
	gChallengeResponse = calcCR(nonce);
	SendPacket_t* pPacket = sessionCreatePacket(0, CHALLENGE_RESPOND, 4, 0, 0, 0);
	write_msblsb(&pPacket->pBuf[9], gChallengeResponse);
	return pPacket;
}

void sendApplicationPacket(uint8_t* pPacket, uint32_t size) {
	gSequenceNumber_tx++;
	SendPacket_t* pAppPacket = malloc(sizeof(SendPacket_t));
	if (pAppPacket == NULL) {
		printf("No memory available!\n");
	}
	pAppPacket->pBuf = pPacket;
	pAppPacket->size = size;
	uint16_t hmac = calcHashMac(pAppPacket->pBuf, pAppPacket->size, APP_MSG);
	pAppPacket = sessionCreatePacket(0, APP_MSG, size, gSessionId, gSequenceNumber_tx, hmac);

	memcpy(&pAppPacket->pBuf[7], pPacket, size);
	network_send(pAppPacket->pBuf, pAppPacket->size);
}

void cbNetworkReceive(uint8_t* pBuffer, uint32_t len) {
	printf("Packet received with length = %d.\r\n", len);
	for (int i = 0; i < len; i++) {
		printf("%02X ",pBuffer[i]);
	}
	printf("\n");
	if (pBuffer[0] == SERVER_CHALLENGE) {
		uint32_t nonce = read_msblsb32bit(&pBuffer[7]);
		SendPacket_t* pPacket = communicator_challengeRespond(nonce);
		network_send(pPacket->pBuf, pPacket->size);
		sessionDestroyPacket(pPacket);
	}

	if (pBuffer[0] == SESSION_ESTABLISHED_) {
		gSessionId = (pBuffer[3] << 8) | pBuffer[4];
		printf("Session created --> SessionId: %d\n\n", gSessionId);
	}
}

void sessionSendHeartbeat() {
	gSequenceNumber_tx++;
	SendPacket_t* pHeartBeat = malloc(sizeof(SendPacket_t));
	if (pHeartBeat == NULL) {
		printf("No memory available!\n");
	}
	pHeartBeat->pBuf = malloc(0);
	uint16_t hmac = calcHashMac(pHeartBeat->pBuf, 0, HEARTBEAT);
	pHeartBeat = sessionCreatePacket(0, HEARTBEAT, 0, gSessionId, gSequenceNumber_tx, hmac);
	network_send(pHeartBeat->pBuf, pHeartBeat->size);
	sessionDestroyPacket(pHeartBeat);
}

void sessionInvalidate() {
	gSequenceNumber_tx++;
	SendPacket_t* pKillSession = malloc(sizeof(SendPacket_t));
	if (pKillSession == NULL) {
		printf("No memory available!\n");
	}
	uint16_t hmac = calcHashMac(pKillSession->pBuf, 0, SESSION_INVALIDATE);
	pKillSession = sessionCreatePacket(0, SESSION_INVALIDATE, 0, gSessionId, gSequenceNumber_tx, hmac);
	network_send(pKillSession->pBuf, pKillSession->size);
}

SendPacket_t* sessionCreatePacket(uint8_t version, uint8_t commandType, uint16_t length, uint16_t sessionId, uint16_t seqNumber, uint16_t hmac) {

	SendPacket_t* pPacket = malloc(sizeof(SendPacket_t));
	if (pPacket == NULL) {
		return NULL;
	}
	uint8_t* pBuffer = malloc(SESSION_HEADER_LENGTH + length);
	if (pBuffer == NULL) {
		return NULL;
	}
	SessionHeader_t* pHeader = (SessionHeader_t*) pBuffer;
	pBuffer[0] = 0;

	switch (commandType) {
	case SESSION_REQUEST:;
		pHeader->sessionRequest = 1;
		write_msblsb(&pBuffer[1], length);
		write_msblsb(&pBuffer[3], sessionId);
		write_msblsb(&pBuffer[5], seqNumber);
		write_msblsb(&pBuffer[7], hmac);
		pPacket->pBuf = (uint8_t*) pBuffer;
		pPacket->size = SESSION_HEADER_LENGTH;
		return pPacket;
		break;

	case CHALLENGE_RESPOND:;
		pHeader->challangeResponse = 1;
		write_msblsb(&pBuffer[1], length);
		write_msblsb(&pBuffer[3], sessionId);
		write_msblsb(&pBuffer[5], seqNumber);
		write_msblsb(&pBuffer[7], hmac);
		pPacket->pBuf = (uint8_t*) pBuffer;
		pPacket->size = SESSION_HEADER_LENGTH + length;
		return pPacket;
		break;

	case HEARTBEAT:
		pHeader->heartbeat = 1;
		write_msblsb(&pBuffer[1], length);
		write_msblsb(&pBuffer[3], sessionId);
		write_msblsb(&pBuffer[5], seqNumber);
		write_msblsb(&pBuffer[7], hmac);
		pPacket->pBuf = (uint8_t*) pBuffer;
		pPacket->size = SESSION_HEADER_LENGTH + length;
		return pPacket;
		break;

	case SESSION_INVALIDATE:
		pHeader->sessionInvalidate = 1;
		write_msblsb(&pBuffer[1], length);
		write_msblsb(&pBuffer[3], sessionId);
		write_msblsb(&pBuffer[5], seqNumber);
		write_msblsb(&pBuffer[7], hmac);
		pPacket->pBuf = (uint8_t*) pBuffer;
		pPacket->size = SESSION_HEADER_LENGTH + length;
		return pPacket;
		break;

	case APP_MSG:
		pBuffer[0] = 0;
		write_msblsb(&pBuffer[1], length);
		write_msblsb(&pBuffer[3], sessionId);
		write_msblsb(&pBuffer[5], seqNumber);
		write_msblsb(&pBuffer[7 + length], hmac);
		pPacket->pBuf = (uint8_t*) pBuffer;
		pPacket->size = SESSION_HEADER_LENGTH + length;
		return pPacket;

	default:
		return (SendPacket_t*) NULL;
		break;
	}

}

void sessionDestroyPacket(SendPacket_t* pPacket) {
	free(pPacket->pBuf);
	free(pPacket);
}

uint16_t calcCR(uint32_t nonce) {
	uint8_t hash[12] = { 0x00, 0x00, 0x00, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x01 }; // hash_calc_start
	write_msblsb32bit(&hash[0], nonce);
	uint16_t hash1 = generateCRC(hash, 12);
	write_msblsb(&hash[0], hash1);
	uint16_t hash2 = generateCRC(hash, 2);
	gChallengeResponse = hash2;
	printf("CR: %d\n", hash2);
	return gChallengeResponse;
}

uint16_t calcHashMac(uint8_t* pBuf, uint32_t len, uint8_t sessionCommand) {
	//HMAC = HASH(HASH(CR | S | H | PL)
	uint8_t hashSize = 17;
	uint8_t* pHash = malloc(hashSize + len);
	if (pHash == NULL) {
		return NULL;
	}
	uint8_t hash[17] = { 0x00, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //(CR | S | H )
	write_msblsb(&hash[0], gChallengeResponse);

	switch (sessionCommand) {
		case HEARTBEAT:
			hash[10] = 1;
			break;
		case SESSION_INVALIDATE:
			hash[10] = 32;
			break;
		case APP_MSG:
			hash[10] = 0;
			break;
		default:
			break;
	}
	write_msblsb(&hash[11], len);
	write_msblsb(&hash[13], gSessionId);
	write_msblsb(&hash[15], gSequenceNumber_tx);
	memcpy(&pHash[0], &hash[0], 17);
	memcpy(&pHash[17], &pBuf[0], len);
	uint16_t hash1 = generateCRC(pHash, hashSize + len);
	uint8_t hash1_arr[2] = { 0 };
	write_msblsb(&hash1_arr[0], hash1);
	uint16_t hash2 = generateCRC(hash1_arr, 2);
	free(pHash);
	return hash2;
}

uint16_t generateCRC (uint8_t* pBuf, uint32_t len) {

	uint16_t crc = 0xFFFF;
	for (int i = 0; i < len; i++) {
		crc = crc ^ pBuf[i];
		for (int j = 0; j < 8; j++) {
			if (crc & 0b1) {
				crc = crc >> 1;
				crc = crc ^ 0xA001;
			} else {
				crc = crc >> 1;
			}
		}
	}

	return crc;
}

void write_msblsb (uint8_t* pBuf, uint16_t val) {
	pBuf[0] = (val & 0xFF00) >> 8;
	pBuf[1] = (val & 0xFF);
}

void write_msblsb32bit(uint8_t* pBuf, uint32_t val) {
	pBuf[0] = (val & 0xFF000000) >> 24;
	pBuf[1] = (val & 0xFF0000) >> 16;
	pBuf[2] = (val & 0xFF00) >> 8;
	pBuf[3] = (val & 0xFF);
}

uint16_t read_msblsb(uint8_t* pBuf) {
	uint16_t result = (pBuf[0] << 8) | (pBuf[1]);
	return result;
}

uint32_t read_msblsb32bit(uint8_t* pBuf) {
	uint32_t result = (pBuf[0] << 24) | (pBuf[1] << 16) | (pBuf[2] << 8) | (pBuf[3] << 0);
	return result;
}
