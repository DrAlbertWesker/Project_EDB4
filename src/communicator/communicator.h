/*
 * communicator.h
 *
 *  Created on: 07.05.2020
 *      Author: nicom
 */

#ifndef SRC_COMMUNICATOR_COMMUNICATOR_H_
#define SRC_COMMUNICATOR_COMMUNICATOR_H_

#include "../network/network.h"

typedef enum {
	TEST_SERVER,
	MAIN_SERVER,
	ECHO_SERVER,
	GAME_SERVER
} Server_e;

typedef enum {
	SESSION_REQUEST,
	CHALLENGE_RESPOND,
	SESSION_INVALIDATE,
	HEARTBEAT,
	APP_MSG
}SessionCommand_e;

typedef struct SendPacket{
	uint8_t* pBuf;
	uint32_t size;
}SendPacket_t;

#pragma pack(1)
typedef struct sessionHeader {
	uint8_t heartbeat:1;
	uint8_t sessionRequest:1;
	uint8_t sessionChallange:1;
	uint8_t challangeResponse:1;
	uint8_t sessionResult:1;
	uint8_t sessionInvalidate:1;
	uint8_t version:2;
}SessionHeader_t;

#pragma pack(pop)

int communicator_connect(Server_e server);

SendPacket_t* sessionCreatePacket(uint8_t version, uint8_t commandType, uint16_t length, uint16_t sessionId, uint16_t seqNumber, uint16_t hmac);

uint32_t getNoace();
void sessionInvalidate();
void sessionSendHeartbeat();
int communicator_createSesson();
uint16_t calcCR(uint32_t nonce);
void sessionDestroyPacket(SendPacket_t* pPacket);
uint16_t generateCRC (uint8_t* pBuf, uint32_t len);
SendPacket_t* communicator_challengeRespond(uint32_t nonce);
void sendApplicationPacket(uint8_t* pPacket, uint32_t size);
uint16_t calcHashMac(uint8_t* pBuf, uint32_t len, uint8_t sessionCommand);

void write_msblsb (uint8_t* pBuf, uint16_t val);
void write_msblsb32bit(uint8_t* pBuf, uint32_t val);
uint16_t read_msblsb(uint8_t* pBuf);
uint32_t read_msblsb32bit(uint8_t* pBuf);

#endif /* SRC_COMMUNICATOR_COMMUNICATOR_H_ */
