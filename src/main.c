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

	printf("##############################################################################################################\n");
	printf("##############################################################################################################\n");
	printf("####                                                                                                      ####\n");
	printf("####   ..####....####...##..##..######..#####.............##.....####............####...######..##...##   ####\n");
	printf("####   .##..##..##..##..##..##....##....##..##...........###....##..##..........##........##....###.###   ####\n");
	printf("####   .##......##..##..##..##....##....##..##..######....##.....####............####.....##....##.#.##   ####\n");
	printf("####   .##..##..##..##...####.....##....##..##............##......##................##....##....##...##   ####\n");
	printf("####   ..####....####.....##....######..#####...........######...##..............####...######..##...##   ####\n");
	printf("####   ................................................................................................   ####\n");
	printf("####                                                                                                      ####\n");
	printf("##############################################################################################################\n");
	printf("##############################################################################################################\n");

	printf("\nControlls: \n");
	printf("Arrow-Keys:                Movement.\n");
	printf("Space:                     Drop food\n");
	printf("Insert:                    Chat-Message\n");
	printf("Space + ESC:               Disconnect from server\n");
	printf("ESC:                       Exit game\n\n");
	Sleep(1000);

	setbuf(stdout, NULL);
	input_service_init(cbInputHandler);
	communicatorConnect(GAME_SERVER);
	if (communicatorCreateSesson() != 0) {
		printf("Failed to create session!\n");
		return -1;
	}
	Sleep(1000);
	PlayerColor_t* pPlayerColor = malloc(sizeof(PlayerColor_t));
	pPlayerColor->red = 255;
	pPlayerColor->green = 255;
	pPlayerColor->blue = 0;
	SendPacket_t* pRegistPacket = createPlayerRegistrationPacket(0xF00D, "NiMa", &playerAvatar_RFU[0], pPlayerColor);
	communicatorSendApplicationPacket(pRegistPacket->pBuf, pRegistPacket->size);
	communicatorDestroyPacket(pRegistPacket);
	free(pPlayerColor);

	uint32_t time_now_s = time(NULL);
	uint32_t time_heartbeat = time_now_s;

	while (gRunning) {
		time_now_s = time(NULL);
		if ((time_now_s - time_heartbeat) > 10) {
			communicatorSendHeartbeat();
			time_heartbeat = time_now_s;
		}
		if (!(((gUp == true) && (gDown == true)) || ((gLeft == true) && (gRight == true)))) {
			if (!(gUp == false && gDown == false && gLeft == false && gRight == false)) {
				SendPacket_t* pControlPacket;
				pControlPacket = createPlayerControlPacket(gUp, gLeft, gRight, gDown);
				communicatorSendApplicationPacket(pControlPacket->pBuf, pControlPacket->size);
				communicatorDestroyPacket(pControlPacket);
			}
   		}
		Sleep(100);
	}
	printf("Exiting...");
	return 0;
}

static void cbInputHandler(InputKeyMask_t m) {
	if (m == INPUT_KEY_MASK_KEY_ESC) {
		gRunning = false;
	}
	gUp = (m & INPUT_KEY_MASK_KEY_UP);
	gRight = (m & INPUT_KEY_MASK_KEY_RIGHT);
	gDown = (m & INPUT_KEY_MASK_KEY_DOWN);
	gLeft = (m & INPUT_KEY_MASK_KEY_LEFT);
	if (m == INPUT_KEY_MASK_KEY_SPACE + INPUT_KEY_MASK_KEY_ESC) {
		communicatorInvalidateSession();
	}
	if (m == INPUT_KEY_MASK_KEY_SPACE) {
		SendPacket_t* pDropFood = createPlayerDropFoodPacket();
		communicatorSendApplicationPacket(pDropFood->pBuf, pDropFood->size);
	}
	if (m == INPUT_KEY_MASK_KEY_INSERT) {
		printf("Enter a chat message and hit ENTER: ");
		char chatMessage[100];
		gets(chatMessage);
		SendPacket_t* pMessage = createPlayerChatPacket(chatMessage);
		communicatorSendApplicationPacket(pMessage->pBuf, pMessage->size);
		communicatorDestroyPacket(pMessage);
	}

	Sleep(100);
}
