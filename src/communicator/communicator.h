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

#pragma pack(1)
typedef struct sessionFlags {
	uint8_t heartbeat:1;
	uint8_t sessionRequest:1;
	uint8_t sessionChallange:1;
	uint8_t challangeResponse:1;
	uint8_t sessionResult:1;
	uint8_t sessionInvalidate:1;
	uint8_t version:2;
}sessionFlags_t;

#pragma pack(pop)

int communicator_connect(Server_e server);
int communicator_createSesson();
void send_heartbeat();
uint8_t* calcHashMacCR(uint8_t* pResponse, uint16_t nonce);
uint16_t calcHashMacHeartbeat();
uint32_t getNoace();
uint16_t generateCRC (uint8_t* pBuf, uint32_t len);
void write_msblsb (uint8_t* pBuf, uint16_t val);
void write_msblsb32bit(uint8_t* pBuf, uint32_t val);
uint16_t read_msblsb(uint8_t* pBuf);
uint32_t read_msblsb32bit(uint8_t* pBuf);

#endif /* SRC_COMMUNICATOR_COMMUNICATOR_H_ */
