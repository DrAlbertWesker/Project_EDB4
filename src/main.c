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

int main(int argc, char** argv) {
	setbuf(stdout, NULL);

	input_service_init(cbInputHandler);
	communicator_connect(GAME_SERVER);
	if (communicator_createSesson() != 0) {
		printf("Failed to create session!\n");
		return -1;
	}
	communicatorRegisterPlayer();
	uint32_t time_now_s = time(NULL);
	uint32_t time_heartbeat = time_now_s;
 	while (gRunning) {
		time_now_s = time(NULL);
		if ((time_now_s - time_heartbeat) > 10) {
			communicatorSendHeartbeat();
			SendPacket_t* pMessage = createPlayerChatPacket("Heartbeat");
			sendApplicationPacket(pMessage->pBuf, pMessage->size);
			sessionDestroyPacket(pMessage);
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
		Sleep(20);
	}
	gUp =  (m & INPUT_KEY_MASK_KEY_UP);
	gRight = (m & INPUT_KEY_MASK_KEY_RIGHT);
	gDown = (m & INPUT_KEY_MASK_KEY_DOWN);
	gLeft = (m & INPUT_KEY_MASK_KEY_LEFT);
	if (!(((gUp == true) && (gDown == true)) || ((gLeft == true) && (gRight == true)))) {
		SendPacket_t* pControlPacket = createPlayerControlPacket(gUp, gLeft, gRight, gDown);
		uint8_t testVal = pControlPacket->pBuf[7];
		printf("Control Command: %d\n", testVal);
		sendApplicationPacket(pControlPacket->pBuf, pControlPacket->size);
		sessionDestroyPacket(pControlPacket);
		printf("Still moving!!!\n");

		Sleep(200);
	}

	if (m == INPUT_KEY_MASK_KEY_DOWN + INPUT_KEY_MASK_KEY_SPACE) {
		sessionInvalidate();
	}
	if (m == INPUT_KEY_MASK_KEY_SPACE) {
		SendPacket_t* pDropFood = createPlayerDropFoodPacket();
		sendApplicationPacket(pDropFood->pBuf, pDropFood->size);
	}

}
