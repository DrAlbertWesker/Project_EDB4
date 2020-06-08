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
	ECHO_SERVER
} Server_e;

#pragma pack(1)
typedef struct header {
	uint8_t version:2;
	uint8_t sessionRequest:1;
	uint8_t challengeRequest:1;
	uint8_t rfu:4;
	uint16_t sessionId;
	uint16_t payloadLen;
} header_t;
#pragma pack(pop)

int communicator_connect(Server_e server);
int communicator_createSesson();


#endif /* SRC_COMMUNICATOR_COMMUNICATOR_H_ */
