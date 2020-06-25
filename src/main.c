#include "input/input_service.h"
#include "windows.h"
#include <time.h>
#include <stdio.h>
#include "communicator/communicator.h"
#include "protocol/protocol.h"

static void cbInputHandler(InputKeyMask_t);

static bool gRunning = true;

int main(int argc, char** argv) {
	setbuf(stdout, NULL);

	input_service_init(cbInputHandler);

	communicator_connect(GAME_SERVER);
	if (communicator_createSesson() != 0) {
		printf("Failed to create session!\n");
		return -1;
	}

	PlayerColor_t color;
	color.red = 255;
	color.green = 255;
	color.blue = 255;
	SendPacket_t* pPacket = createPlayerRegistrationPacket(0x1234, "NiMa");

	uint32_t time_now_s = time(NULL);
	uint32_t time_heartbeat = time_now_s;


	while (gRunning) {

		time_now_s = time(NULL);
		if ((time_now_s - time_heartbeat) > 10) {
			sessionSendHeartbeat();
			printf("Sending heartbeat\n");
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
	if (m & INPUT_KEY_MASK_KEY_SPACE){
		uint8_t test[3] = {0, 1, 2};
		printf("TEST");
	}
}
