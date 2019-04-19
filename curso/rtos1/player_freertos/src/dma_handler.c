#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "portable.h"
#include "portmacro.h"
#include "semphr.h"
#include "task.h"
#include <stdint.h>
#include "board.h"
#include "dma_handler.h"


sample_type dmabuf[HALF_DMA_BUFSIZ*2] = {0};
uint32_t dmaTransferComplete = 0;
static uint8_t dmaChannelNum_I2S_Tx;

//TODO: implement semaphores
/**
 * @brief	DMA interrupt handler sub-routine
 * @return	Nothing
 */
__attribute__ ((section(".after_vectors")))
void DMA_IRQHandler(void) {
	static unsigned char ucLocalTickCount = 0;
	static signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE; //FIXME? portBASE_TYPE defined but BaseType_t not?

	if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNum_I2S_Tx) == SUCCESS) {
		dmaChannelNum_I2S_Tx = Chip_GPDMA_GetFreeChannel(LPC_GPDMA,
				I2S_DMA_TX_CHAN);

		Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum_I2S_Tx,
				(uint32_t) &dmabuf[send_index], //src is the bufffer
				I2S_DMA_TX_CHAN, GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA,
				HALF_DMA_BUFSIZ,
				DMA_CONFIG); //Big Endian
		//update the index and populate the new region
		if (send_index) {
			send_index = 0;
		} else {
			send_index = HALF_DMA_BUFSIZ;
		}
		dmaTransferComplete++;
	} else {
		/* Process error here */
		printf("Error escribiendo buffer i2s\n");
	}

	//xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);


	/* If xHigherPriorityTaskWoken was set to true you we should yield. */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
