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
	CMD_SESSION_REQUEST,
	CMD_CHALLENGE_RESPOND,
	CMD_SESSION_INVALIDATE,
	CMD_HEARTBEAT,
	CMD_APP_MSG
}SessionCommand_e;

typedef enum {
	HDR_SESSION_HEARTBEAT = 1,
	HDR_SESSION_REQUEST = 2,
	HDR_SESSION_CHALLENGE = 4,
	HDR_CHALLENGE_RESPOND = 8,
	HDR_SESSION_RESULT = 16,
	HDR_SESSION_INVALIDATE = 32,
}SessionHeader_e;

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

int communicatorConnect(Server_e server);
void communicatorInvalidateSession();
void communicatorSendHeartbeat();
int  communicatorCreateSesson();
void communicatorDestroyPacket(SendPacket_t* pPacket);
void communicatorSendApplicationPacket(uint8_t* pPacket, uint32_t size);
SendPacket_t* communicator_challengeRespond(uint32_t nonce);
SendPacket_t* communicatorCreateSessionPacket(uint8_t version, uint8_t commandType, uint16_t length, uint16_t sessionId, uint16_t seqNumber, uint16_t hmac);

uint16_t calcCR(uint32_t nonce);
uint16_t generateCRC (uint8_t* pBuf, uint32_t len);
uint16_t calcHashMac(uint8_t* pBuf, uint32_t len, uint8_t sessionCommand);
void write_msblsb (uint8_t* pBuf, uint16_t val);
void write_msblsb32bit(uint8_t* pBuf, uint32_t val);
uint16_t read_msblsb(uint8_t* pBuf);
uint32_t read_msblsb32bit(uint8_t* pBuf);

#endif /* SRC_COMMUNICATOR_COMMUNICATOR_H_ */
