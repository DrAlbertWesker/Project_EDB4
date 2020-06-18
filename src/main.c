#include "input/input_service.h"
#include "windows.h"
#include <stdio.h>
#include "communicator/communicator.h"

static void cbInputHandler(InputKeyMask_t);

static bool gRunning = true;

int main(int argc, char** argv) {
	setbuf(stdout, NULL);

	input_service_init(cbInputHandler);

	communicator_connect(GAME_SERVER);
	communicator_createSesson();

	printf("%d\r\n", sizeof(sessionFlags_t));
	while(gRunning) {

		Sleep(1000);
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
