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

static void cbNetworkReceive(uint8_t* pBuffer, uint32_t len);
uint16_t sessionID;

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

	uint8_t packet[9] = {0};
	sessionFlags_t* pHeader = (sessionFlags_t*) &packet[0];
	pHeader ->version = 0;
	pHeader->sessionRequest = 1;
	//Todo: Set state wating for challenge
	sessionID = 0;
	network_send(packet, 9);
	while (!sessionID) {
		Sleep(10);
	}

	// ... Wait until session created

	return 0;
}

void cbNetworkReceive(uint8_t* pBuffer, uint32_t len) {
	printf("Packet received with length = %d.\r\n", len);
	for (int i = 0; i < len; i++) {
		printf("%02X ",pBuffer[i]);
	}
	printf("\n");
	if (pBuffer[0] == 0x04) { //Todo: nur wenn state waring for shallenge
		uint32_t nonce = (pBuffer[7] << 24) | (pBuffer[8] << 16) | (pBuffer[9] << 8) | (pBuffer[10]); //Gegen Funktion ersetzen
		printf("\nNonce = %d\n", nonce);
		uint8_t hash[12] = { 0x00, 0x00, 0x00, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x01 };
		memcpy(hash, &pBuffer[7], 4);
		uint16_t hash1 = generateCRC(hash, 12);
		hash[0] = hash1 >> 8;
		hash[1] = hash1 & 0xFF;
		uint16_t hash2 = generateCRC(hash, 2);
		printf("%d\n", hash2);
		uint8_t response[13] = { 0 };
		response[0] = 0x08;
		response[2] = 4;
		response[9] = hash2 >> 8; 			//funktion für paketierung erstellen
		response[10] = hash2 & 0xFF;
		network_send(response, 13);
	}

	if (pBuffer[0] == 0x10) {
		sessionID = (pBuffer[3] << 8) | pBuffer[4];
		printf("SessionID: %d\n", sessionID);
	}
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

