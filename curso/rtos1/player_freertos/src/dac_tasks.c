/*
 * dac_tasks.c
 *
 *  Created on: Apr 19, 2019
 *      Author: dc740
 */
#include <stdint.h>
#include "board.h"
#include "dma_handler.h"
#include "audio_generation.h"

void fillBufferTask(void* taskParmPtr) {
	uint32_t ulNotificationValue;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10000); //wait for 10 seconds. This is a symbolic value

	xTaskToNotifyAboutDMA = xTaskGetCurrentTaskHandle();

	//Trigger the transfer
	printf("Iniciando primera trama\n");
	Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum_I2S_Tx, (uint32_t) &dmabuf[0], //src is the bufffer
			I2S_DMA_TX_CHAN, GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
			HALF_DMA_BUFSIZ,
			DMA_CONFIG);
	printf("Dejando el control a la interrupcion de DMA\n");

	while (1) {
		/* Wait for the transmission to complete. */
		ulNotificationValue = ulTaskNotifyTake( pdFALSE, xMaxBlockTime);

		if (ulNotificationValue == 1) {
			/* The transmission ended as expected.
			 * So lets populate the next part of the buffer
			 */
			populate_wave(send_index);
		} else {
			/* The call to ulTaskNotifyTake() timed out. */
		}
	}
}
