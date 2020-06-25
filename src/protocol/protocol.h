/*
 * protocol.h
 *
 *  Created on: 22.06.2020
 *      Author: nicom
 */

#ifndef SRC_PROTOCOL_PROTOCOL_H_
#define SRC_PROTOCOL_PROTOCOL_H_
#include <inttypes.h>
#include <stdbool.h>

#define HEADER_LENGTH 7


typedef enum {
	REGISTER_PLAYER = 0x01,
	PLAYER_CONTROL  = 0x0A,
	DROP_FOOD		= 0x14,
	REQST_PLYR_INFO	= 0x1E,
	CHAT_MSG		= 0x28
} CommandId_e;

typedef enum {
	PlAYER_NAME = 1,
	PLAYER_IMAGE = 2,
	PLAYER_COLOR = 3
}RegisterRequest_e;

typedef struct {
	uint8_t version:2;
	uint8_t RFU:4;
	uint8_t Response:1;
	uint8_t Request:1;
	uint16_t length: 16;
	uint16_t commandId: 16;
	uint16_t transactionId: 16;
}PacketHeader_t;

typedef struct {
	uint8_t type;
	uint16_t length;
	uint8_t* pData;
}tlv_t;

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} PlayerColor_t;

//SendPacket_t* createPlayerRegistrationPacket(uint16_t transactionId, char* playername);
//SendPacket_t* createPlayerControlPacket(bool up, bool right, bool down, bool left);
//SendPacket_t* createPlayerDropFoodPacket(void);
//SendPacket_t* createPlayerChatPacket(char* message);
void sendHeartbeat();

//void destroySendPacket(SendPacket_t* pPacket);

#endif /* SRC_PROTOCOL_PROTOCOL_H_ */
