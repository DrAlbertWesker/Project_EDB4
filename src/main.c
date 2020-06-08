#include "input/input_service.h"
#include "windows.h"
#include <stdio.h>
#include "communicator/communicator.h"

//test comment for git useage
static void cbInputHandler(InputKeyMask_t);

static bool gRunning = true;

int main(int argc, char** argv) {
	setbuf(stdout, NULL);

	input_service_init(cbInputHandler);

	if (communicator_connect(ECHO_SERVER) != 0) {
		printf("Fehler");
		return -1;
	}

	printf("%d\r\n", sizeof(header_t));
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

	}
}
