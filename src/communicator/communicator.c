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
#define SESSION_ESTABLISHED							0x10


static void cbNetworkReceive(uint8_t* pBuffer, uint32_t len);
static uint16_t sessionId;
uint16_t sequenceNumber_tx;
uint16_t sequenceNumber_rx;
uint16_t challengeResponse;

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

	sessionId = 0;
	uint8_t packet[9] = {0};
	sessionFlags_t* pHeader = (sessionFlags_t*) &packet[0];
	pHeader->version = 0;
	pHeader->sessionRequest = 1;
	network_send(packet, 9);
	while (sessionId == 0) {
		Sleep(10);
	}
	return 0;
}

void cbNetworkReceive(uint8_t* pBuffer, uint32_t len) {
	printf("Packet received with length = %d.\r\n", len);
	for (int i = 0; i < len; i++) {
		printf("%02X ",pBuffer[i]);
	}
	printf("\n");
	if (pBuffer[0] == SERVER_CHALLENGE) {
		uint32_t nonce = read_msblsb32bit(&pBuffer[7]);
		printf("\nNonce = %d\n", nonce);
		uint8_t hash[12] = { 0x00, 0x00, 0x00, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x01 }; // hash_calc_start
		memcpy(hash, &pBuffer[7], 4);
		uint16_t hash1 = generateCRC(hash, 12);
		write_msblsb(&hash[0], hash1);
		uint16_t hash2 = generateCRC(hash, 2);
		challengeResponse = hash2;
		printf("CR: %d\n", hash2);
		uint8_t response[13] = { 0 };
		response[0] = 0x08;
		response[2] = 4;
		write_msblsb(&response[9], hash2); // hash_calc_end
		network_send(response, 13);
	}

	if (pBuffer[0] == SESSION_ESTABLISHED) {
		sessionId = (pBuffer[3] << 8) | pBuffer[4];
		printf("SessionId: %d\n", sessionId);
	}
}


uint8_t* calcHashMacCR(uint8_t *pResponse, uint16_t nonce) {

		//TODO
		return NULL;
}


void sendHeartbeat() {
	uint8_t buffer[9] = { 0 };			// buffer = HEARTBEAT, length (0 --> no PL), sessionId, sequenceNumber, hmac
	buffer[0] = 1;
	write_msblsb(&buffer[3], sessionId);
	write_msblsb(&buffer[5], ++sequenceNumber_tx);
	uint16_t hmac = calcHashMacHeartbeat();
	write_msblsb(&buffer[7], hmac);
	network_send(&buffer[0], 9);
}

uint16_t calcHashMacHeartbeat() {

	uint8_t hash[17] = { 0x00, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //(CR | S | H |
	write_msblsb(&hash[0], challengeResponse);
	hash[10] = 1;
	hash[11] = 0;
	hash[12] = 0;
	write_msblsb(&hash[13], sessionId);
	write_msblsb(&hash[15], sequenceNumber_tx);
	uint16_t hash1 = generateCRC(hash, 17);
	uint8_t hash1_arr[2] = { 0 };
	write_msblsb(&hash1_arr[0], hash1);
	uint16_t hash2 = generateCRC(hash1_arr, 2);
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
