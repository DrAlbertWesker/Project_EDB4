#include "input/input_service.h"
#include "windows.h"
#include <time.h>
#include <stdio.h>
#include "communicator/communicator.h"
#include "protocol/protocol.h"

static void cbInputHandler(InputKeyMask_t);

static bool gRunning = true;
static bool gUp = false;
static bool gRight = false;
static bool gDown = false;
static bool gLeft = false;

static uint8_t playerAvatar_RFU[128] = { 0 };

int main(int argc, char** argv) {
	setbuf(stdout, NULL);

	input_service_init(cbInputHandler);
	communicatorConnect(GAME_SERVER);
	if (communicatorCreateSesson() != 0) {
		printf("Failed to create session!\n");
		return -1;
	}

	PlayerColor_t* pPlayerColor = malloc(sizeof(PlayerColor_t));
	pPlayerColor->red = 255;
	pPlayerColor->green = 255;
	pPlayerColor->blue = 0;
	SendPacket_t* pRegistPacket = createPlayerRegistrationPacket(0x80, "NiMa", &playerAvatar_RFU[0] , pPlayerColor);
	communicatorSendApplicationPacket(pRegistPacket->pBuf, pRegistPacket->size);
	communicatorDestroyPacket(pRegistPacket);

	uint32_t time_now_s = time(NULL);
	uint32_t time_heartbeat = time_now_s;
 	while (gRunning) {
		time_now_s = time(NULL);
		if ((time_now_s - time_heartbeat) > 10) {
			communicatorSendHeartbeat();
			SendPacket_t* pMessage = createPlayerChatPacket("Heartbeat");
			communicatorSendApplicationPacket(pMessage->pBuf, pMessage->size);
			communicatorDestroyPacket(pMessage);
			time_heartbeat = time_now_s;
		}
 		Sleep(10);
	}
	printf("Exiting...");
	return 0;
}

static void cbInputHandler(InputKeyMask_t m) {
	printf("Input event: %d.\r\n", m);
	if (m == INPUT_KEY_MASK_KEY_ESC) {
		gRunning = false;
	}
	gUp =  (m & INPUT_KEY_MASK_KEY_UP);
	gRight = (m & INPUT_KEY_MASK_KEY_RIGHT);
	gDown = (m & INPUT_KEY_MASK_KEY_DOWN);
	gLeft = (m & INPUT_KEY_MASK_KEY_LEFT);
	if (!(((gUp == true) && (gDown == true)) || ((gLeft == true) && (gRight == true)))) {
	SendPacket_t* pControlPacket;
	pControlPacket = createPlayerControlPacket(gUp, gLeft, gRight, gDown);
	communicatorSendApplicationPacket(pControlPacket->pBuf, pControlPacket->size);
	Sleep(500);
	communicatorDestroyPacket(pControlPacket);

	}

	if (m == INPUT_KEY_MASK_KEY_SPACE + INPUT_KEY_MASK_KEY_ESC) {
		communicatorInvalidateSession();
	}
	if (m == INPUT_KEY_MASK_KEY_SPACE) {
		SendPacket_t* pDropFood = createPlayerDropFoodPacket();
		communicatorSendApplicationPacket(pDropFood->pBuf, pDropFood->size);
	}

}
