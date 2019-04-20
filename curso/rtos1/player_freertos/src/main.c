#include "../../player/inc/sd_spi.h"   // <= own header (optional)

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#include "board.h"
#include "wavData.h"
#include "dma_handler.h"
#include "dac_setup.h"
#include "dac_tasks.h"
#include <string.h>
#include <stdint.h>

//FreeRTOS tasks
StackType_t myTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t myTaskTCB;

void mySetupTask(void * pvParameters) {
	//Setup steps
	//Populate the entire buffer. Then we will send one half, and the
	// DMA irq will trigger the next one.
	populate_wave(0);
	populate_wave(HALF_DMA_BUFSIZ);
	initDac();
	// Start the other tasks
	xTaskCreate(fillBufferTask, "fillBufferTask",
	configMINIMAL_STACK_SIZE, NULL,
	tskIDLE_PRIORITY + 1, xTaskToNotifyAboutDMA);

	// Delete Setup task (ourselves).
	vTaskDelete(NULL);
}

int main(void) {
	xTaskCreateStatic(mySetupTask, "mySetupTask", configMINIMAL_STACK_SIZE,
	NULL,
	tskIDLE_PRIORITY + 1, myTaskStack, &myTaskTCB);
	vTaskStartScheduler();
	while (1)
		;
	return 0;
}
